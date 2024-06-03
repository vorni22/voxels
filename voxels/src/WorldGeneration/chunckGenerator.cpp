#include "chunckGenerator.h"

// chunck colection
std::vector<Chunck*> chunks;

// chunck generator
std::set<std::pair<int, int>> loaded_chuncks;
glm::ivec3 oldPos;

std::vector<chunck_buffer*> Buffers;
std::vector<int> freeIndices;
bool exception_flag = false;

int renderDistance;
glm::vec3 generatorPos = glm::vec3(0.0f, 0.0f, 0.0f);

int MAX_VERTEX_COUNT = xLen * yLen * zLen;
float blockSize;
bool initialized = false;

vlibs::homogeneous_thread_pool<Chunck*> thread_pool(ChunckGenerator::get_optimal_threads(), 1000000);

void ChunckGenerator::init(int _renderDist, float _blockSize){
	int len = 2 * _renderDist + 1;

	unsigned int VAO_cap = (unsigned int)MAX_VERTEX_COUNT / 2u + 1u;

	for (int i = 0; i < len * len; i++) {
		chunck_buffer* CHUNCK_BUFFER = new chunck_buffer(VAO_cap, VAO_cap);
		Buffers.push_back(CHUNCK_BUFFER);
		freeIndices.push_back(i);
	}

	renderDistance = _renderDist;
	blockSize = _blockSize;

	// get relative position
	glm::ivec3 relativePos = getPos();
	oldPos = relativePos;
	
	for (int x = relativePos.x - renderDistance; x <= relativePos.x + renderDistance; x++) {
		for (int z = relativePos.z - renderDistance; z <= relativePos.z + renderDistance; z++) {
			int bufferId = getBuffer();

			GenerateChunck(x, 0, z, bufferId);
			loaded_chuncks.insert({ x, z });
		}
	}

	thread_pool.wait_for_tasks();
	bool sw = true;
	while (sw) {
		Chunck* _chunck;
		thread_pool.get_latest_return(_chunck, sw);

		if (sw) {
			_chunck->init();
		}
	}

	std::cout << "Chunck generator initialized" << std::endl;

	initialized = true;
}

void ChunckGenerator::delAll() {
	if(!initialized) return;

	thread_pool.wait_for_tasks();

	std::cout << "Deleting chuncks" << std::endl;
	for (int i = 0; i < chunks.size(); i++) {
		delete chunks[i];
	}
	std::cout << "Deleting buffers" << std::endl;
	for (int i = 0; i < Buffers.size(); i++) {
		delete Buffers[i];
	}
	chunks.clear();
	Buffers.clear();
	freeIndices.clear();
}

void ChunckGenerator::Draw(Shader* shader, Camera* camera, bool newPerspective){
	if (!initialized) return;

	shader->activate();
	//shader->set3Float("lightPos", lightPos);
	glm::vec3 lightDir = glm::vec3(0.0f, 0.5f, sqrt(3.0f) / 2.0f);
	shader->set3Float("lightDir", lightDir);
	shader->setMat4("view", camera->viewMatrix());
	if (newPerspective) {
		shader->setMat4("projection", camera->perspectiveMatrix());
	}

	glEnable(GL_CULL_FACE);

	// draw non-transparent faces
	for (int i = 0; i < chunks.size(); i++) {
		if (chunks[i]->isLoaded()) {
			chunks[i]->Draw_opaque(shader, camera, newPerspective);
		}
	}

	glDisable(GL_CULL_FACE);

	// draw transparent faces
	for (int i = 0; i < chunks.size(); i++) {
		if (chunks[i]->isLoaded()) {
			chunks[i]->Draw_transparent(shader, camera, newPerspective);
		}
	}
}

void ChunckGenerator::changeRenderDistance(int newRenderDist){
	
}

void ChunckGenerator::update(){
	if (!initialized) return;

	int cnt = 0;
	Chunck* _chunck; bool sw = true;
	while (sw && cnt < 6) {
		thread_pool.get_latest_return(_chunck, sw);

		if (sw) {
			_chunck->init();
			cnt++;
		}
	}

	// get chunck position
	glm::ivec3 Pos = getPos();
	int len = 2 * renderDistance + 1;

	oldPos = Pos;
	exception_flag = false;

	// delete all chuncks out of range

	for (int i = 0; i < (int)chunks.size(); i++) {

		chunks[i]->uptadateRelativePos(Pos);
		glm::ivec3 relativePos = chunks[i]->getRelativePos();

		if (chunks[i]->isFailed()) {
			// chunck failed
			delete chunks[i];
			chunks.erase(chunks.begin() + i);
			loaded_chuncks.erase({ Pos.x + relativePos.x, Pos.z + relativePos.z });
			i--;
			continue;
		}

		if (!chunks[i]->isLoaded()) {
			exception_flag = true;
			continue;
		}

		if (
			relativePos.x > renderDistance || relativePos.x < -renderDistance ||
			relativePos.z > renderDistance || relativePos.z < -renderDistance
		) {
			// chunck out of range
			delete chunks[i];
			chunks.erase(chunks.begin() + i);
			loaded_chuncks.erase({Pos.x + relativePos.x, Pos.z + relativePos.z });
			i--;
		}
	}

	cnt = 0;

	// spawn new chuncks in the new range
	for (int x = Pos.x - renderDistance; x <= Pos.x + renderDistance; x++) {
		for (int z = Pos.z - renderDistance; z <= Pos.z + renderDistance; z++) {
			if (freeIndices.size() == 0) {
				return;
			}
			
			if (!loaded_chuncks.count({ x, z })) {

				loaded_chuncks.insert({ x, z });
				int bufferID = getBuffer();
				GenerateChunck(x, 0, z, bufferID);

				exception_flag = true;
				cnt++;

				if(cnt == 10)
					return;
			}
		}
	}
}

void ChunckGenerator::updatePos(glm::vec3 newPos){
	generatorPos = newPos;
}

glm::ivec3 ChunckGenerator::getPos() {
	return glm::ivec3(
		int(generatorPos.x / (int(xLen) * blockSize)),
		int(generatorPos.y / (int(yLen) * blockSize)),
		int(generatorPos.z / (int(zLen) * blockSize))
	);
}

unsigned int ChunckGenerator::get_optimal_threads() {
	unsigned int tot = std::thread::hardware_concurrency();

	if (tot <= 3) {
		return 1;
	}

	if (tot <= 6) {
		return 2;
	}

	if (tot <= 10) {
		return 5;
	}

	if (tot <= 12) {
		return 8;
	}

	if (tot <= 15) {
		return 11;
	}

	if (tot <= 18) {
		return 14;
	}

	return 16;
}

void ChunckGenerator::GenerateChunck(int x, int y, int z, int _bufferId){
	Chunck *newChunck = new Chunck(x, y, z, blockSize, "0", Buffers[_bufferId], _bufferId, &reuseBuffer, &thread_pool);

	newChunck->uptadateRelativePos(getPos());
	chunks.push_back(newChunck);
}

int ChunckGenerator::getBuffer(){ 
	int ret = freeIndices[freeIndices.size() - 1];
	freeIndices.pop_back();
	return ret;
}

void ChunckGenerator::reuseBuffer(int i){
	freeIndices.push_back(i);
}

