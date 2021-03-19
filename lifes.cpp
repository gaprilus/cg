#include "lifes.hpp"
#include <stdio.h>
#include <iostream>
#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

void Lifes::initializeGL(GLuint program, int quantity) {
  terminateGL();

  // Start pseudo-random number generator
  auto seed{std::chrono::steady_clock::now().time_since_epoch().count()};
  m_randomEngine.seed(seed);

  m_program = program;
  m_colorLoc = glGetUniformLocation(m_program, "color");
  m_rotationLoc = glGetUniformLocation(m_program, "rotation");
  m_scaleLoc = glGetUniformLocation(m_program, "scale");
  m_translationLoc = glGetUniformLocation(m_program, "translation");

  // Create Lifes
  m_lifes.clear();
  m_lifes.resize(quantity * fase_atual/2);

  for (auto &life : m_lifes) {
    life = createLife();

    // Make sure the life won't collide with the ship
    do {
      life.m_translation = {m_randomDist(m_randomEngine),
                                m_randomDist(m_randomEngine)};
    } while (glm::length(life.m_translation) < 0.5f);
  }
}

void Lifes::paintGL() {
  glUseProgram(m_program);

  for (auto &life : m_lifes) {
    glBindVertexArray(life.m_vao);

    glUniform4fv(m_colorLoc, 1, &life.m_color.r);
    glUniform1f(m_scaleLoc, life.m_scale);
    glUniform1f(m_rotationLoc, life.m_rotation);

    for (auto i : {-2, 0, 2}) {
      for (auto j : {-2, 0, 2}) {
        glUniform2f(m_translationLoc, life.m_translation.x + j,
                    life.m_translation.y + i);

        glDrawArrays(GL_TRIANGLE_FAN, 0, life.m_polygonSides + 2);
     
      }
    }

    glBindVertexArray(0);
  }

  glUseProgram(0);
}

void Lifes::terminateGL() {
  for (auto life : m_lifes) {
    glDeleteBuffers(1, &life.m_vbo);
    glDeleteVertexArrays(1, &life.m_vao);
  }
}

void Lifes::update(const Ship &ship, float deltaTime) {
  for (auto &life : m_lifes) {
    life.m_translation -= ship.m_velocity * deltaTime;
    life.m_rotation = glm::wrapAngle(
    life.m_rotation + life.m_angularVelocity * deltaTime);
    life.m_translation += life.m_velocity * deltaTime;

    // Wrap-around
    if (life.m_translation.x < -1.0f) life.m_translation.x += 2.0f;
    if (life.m_translation.x > +1.0f) life.m_translation.x -= 2.0f;
    if (life.m_translation.y < -1.0f) life.m_translation.y += 2.0f;
    if (life.m_translation.y > +1.0f) life.m_translation.y -= 2.0f;
  }
}

Lifes::Life Lifes::createLife(glm::vec2 translation,
                                              float scale) {
  Life life;

  auto &re{m_randomEngine};  // Shortcut

  // Randomly choose the number of sides
  std::uniform_int_distribution<int> randomSides(6, 20);
  life.m_polygonSides = randomSides(re);

  // Choose a random color (actually, a grayscale)
  std::uniform_real_distribution<float> randomIntensity(0.6f, 1.0f);
  life.m_color = glm::vec4(1) * randomIntensity(re);

  life.m_color.a = randomIntensity(re);
  life.m_color.b= randomIntensity(re);
  life.m_color.g= randomIntensity(re);
  
  life.m_rotation = 0.0f;
  life.m_scale = scale;
  life.m_translation = translation;

  float fator=(fase_atual+0.5)/2;
  // Choose a random angular velocity
  life.m_angularVelocity = m_randomDist(re) * fator;

  // Choose a random direction
  //glm::vec2 direction{m_randomDist(re), m_randomDist(re)};
  
  glm::vec2 direction{m_randomDist(re), m_randomDist(re)};
  life.m_velocity = (glm::normalize(direction) /7.0f) ;
  
  // Create geometry
  std::vector<glm::vec2> positions(0);
  positions.emplace_back(0, 0);
  
  auto step{M_PI * 2 / life.m_polygonSides};
  std::uniform_real_distribution<float> randomRadius(0.8f, 1.0f);
  for (auto angle : iter::range(0.0, M_PI * 2, step)) {
    auto radius{randomRadius(re)};
    positions.emplace_back(radius * std::cos(angle), radius * std::sin(angle));
  }
  positions.push_back(positions.at(1));

  // Generate VBO
  glGenBuffers(1, &life.m_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, life.m_vbo);
  glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
               positions.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  glGenVertexArrays(1, &life.m_vao);

  // Bind vertex attributes to current VAO
  glBindVertexArray(life.m_vao);

  glBindBuffer(GL_ARRAY_BUFFER, life.m_vbo);
  glEnableVertexAttribArray(positionAttribute);
  glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  glBindVertexArray(0);

  return life;
}
