#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(60);
	ofSetWindowTitle("openFrameworks");

	ofBackground(0);
	ofSetLineWidth(2);
	ofEnableDepthTest();

	this->font.loadFont("fonts/Kazesawa-Bold.ttf", 50, true, true, true);
}

//--------------------------------------------------------------
void ofApp::update() {

	ofSeedRandom(39);
}

//--------------------------------------------------------------
void ofApp::draw() {

	this->cam.begin();
	ofRotateX(180);

	vector<string> word_list = {
		"0", "1", "2", "3",
		"6", "5", "4",
		"7", "8", "9", "0"
	};

	for (int x = -180; x <= 180; x += 60) {

		for (int y = -180; y <= 180; y += 60) {

			int word_index = ofMap(ofNoise(ofRandom(1000), ofGetFrameNum() * 0.003), 0, 1, 0, word_list.size());
			auto word = word_list[word_index];

			int sample_count = 120;
			vector<ofPath> word_path = this->font.getStringAsPoints(word, true, false);
			for (int word_index = 0; word_index < word_path.size(); word_index++) {

				vector<ofPolyline> outline = word_path[word_index].getOutline();
				for (int outline_index = 0; outline_index < outline.size(); outline_index++) {

					outline[outline_index] = outline[outline_index].getResampledByCount(sample_count);
					vector<glm::vec3> vertices = outline[outline_index].getVertices();
					vector<glm::vec2> mesh_vertices;
					vector<glm::vec3> base_location_list;
					vector<float> rad_list;

					for (int vertices_index = 0; vertices_index < vertices.size(); vertices_index++) {

						auto base_location = glm::vec3(x, y, 0);
						base_location += glm::vec3(this->font.stringWidth(word_list[word_index]) * 0.5, this->font.stringHeight(word_list[word_index]) * 0.5, 0);

						auto noise_value = ofNoise(
							x * 0.005 + vertices[vertices_index].x * 0.001,
							(y + vertices[vertices_index].y) * 0.001 + ofGetFrameNum() * 0.006);
						float param = 0.5;
						if (noise_value < 0.3) { param = ofMap(noise_value, 0, 0.3, 0, 0.5); }
						if (noise_value > 0.7) { param = ofMap(noise_value, 0.7, 1, 0.5, 1); }

						auto rad = ofMap(param, 0, 1, -PI * 3, PI * 3);
						auto location = vertices[vertices_index] - glm::vec2(this->font.stringWidth(word_list[word_index]) * 0.5, this->font.stringHeight(word_list[word_index]) * 0.5);

						mesh_vertices.push_back(location);
						rad_list.push_back(rad);
						base_location_list.push_back(base_location);
					}

					ofMesh face, line;
					ofColor face_color, line_color;
					line.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINES);
					for (int k = 0; k < mesh_vertices.size(); k++) {

						auto rotate = glm::rotate(glm::mat4(), rad_list[k], glm::vec3(0, 1, 0));

						if (rad_list[k] != 0) {

							face_color.setHsb(ofMap(rad_list[k], -PI * 3, PI * 3, 0, 255), 255, 255);
							line_color = ofColor(239);
						}
						else {

							face_color = ofColor(239, 128);
							line_color = ofColor(239);
						}

						face.addVertex(glm::vec4(base_location_list[k], 0) + glm::vec4(mesh_vertices[k], -5, 0) * rotate);
						face.addVertex(glm::vec4(base_location_list[k], 0) + glm::vec4(mesh_vertices[k], 5, 0) * rotate);

						face.addColor(ofColor(face_color, 128));
						face.addColor(ofColor(face_color, 128));

						line.addVertex(glm::vec4(base_location_list[k], 0) + glm::vec4(mesh_vertices[k], -5, 0) * rotate);
						line.addVertex(glm::vec4(base_location_list[k], 0) + glm::vec4(mesh_vertices[k], 5, 0) * rotate);

						line.addColor(face_color);
						line.addColor(face_color);

						if (k > 0) {

							face.addIndex(face.getNumVertices() - 1); face.addIndex(face.getNumVertices() - 2); face.addIndex(face.getNumVertices() - 4);
							face.addIndex(face.getNumVertices() - 1); face.addIndex(face.getNumVertices() - 3); face.addIndex(face.getNumVertices() - 4);

							line.addIndex(line.getNumVertices() - 1); line.addIndex(line.getNumVertices() - 3);
							line.addIndex(line.getNumVertices() - 2); line.addIndex(line.getNumVertices() - 4);
						}
					}

					face.addIndex(face.getNumVertices() - 1); face.addIndex(0); face.addIndex(1);
					face.addIndex(face.getNumVertices() - 1); face.addIndex(face.getNumVertices() - 2); face.addIndex(2);

					line.addIndex(line.getNumVertices() - 1); line.addIndex(1);
					line.addIndex(line.getNumVertices() - 2); line.addIndex(0);

					face.draw();
					line.drawWireframe();
				}
			}
		}
	}

	this->cam.end();
}

//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(720, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}