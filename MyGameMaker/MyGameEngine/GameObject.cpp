#include "GameObject.h"
#include <GL/glew.h>
#include <glm/gtc/epsilon.hpp>

const int CHECKERS_WIDTH = 64;
const int CHECKERS_HEIGHT = 64;
GLubyte checkerImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];
GLuint checker_texture_id;

GameObject::GameObject(const std::string& name) : name(name), cachedComponentType(typeid(Component)) {
    AddComponent<TransformComponent>();
}

void deleteCheckerTexture() {
    if (checker_texture_id) {
        glDeleteTextures(1, &checker_texture_id);
        checker_texture_id = 0;
    }
}

// Crea la textura de tablero y devuelve el ID de la textura
void CheckerTexture(bool hasCreatedCheckerImage) {
    for (int i = 0; i < CHECKERS_HEIGHT; i++) {
        for (int j = 0; j < CHECKERS_WIDTH; j++) {
            int c = ((((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255);
            checkerImage[i][j][0] = (GLubyte)c; // Rojo
            checkerImage[i][j][1] = (GLubyte)c; // Verde
            checkerImage[i][j][2] = (GLubyte)c; // Azul
            checkerImage[i][j][3] = (GLubyte)255; // Opacidad completa
        }
    }

    deleteCheckerTexture();
    glGenTextures(1, &checker_texture_id);
    glBindTexture(GL_TEXTURE_2D, checker_texture_id);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);
}

inline static void glVertex3(const vec3& v) { glVertex3dv(&v.x); }
static void drawWiredQuad(const vec3& v0, const vec3& v1, const vec3& v2, const vec3& v3) {
    glBegin(GL_LINE_LOOP);
    glVertex3(v0);
    glVertex3(v1);
    glVertex3(v2);
    glVertex3(v3);
    glEnd();
}
static void drawBoundingBox(const BoundingBox& bbox) {
    glLineWidth(2.0);
    drawWiredQuad(bbox.v000(), bbox.v001(), bbox.v011(), bbox.v010());
    drawWiredQuad(bbox.v100(), bbox.v101(), bbox.v111(), bbox.v110());
    drawWiredQuad(bbox.v000(), bbox.v001(), bbox.v101(), bbox.v100());
    drawWiredQuad(bbox.v010(), bbox.v011(), bbox.v111(), bbox.v110());
    drawWiredQuad(bbox.v000(), bbox.v010(), bbox.v110(), bbox.v100());
    drawWiredQuad(bbox.v001(), bbox.v011(), bbox.v111(), bbox.v101());
}

void GameObject::draw() const {
    glPushMatrix();
    glMultMatrixd(GetComponent<TransformComponent>()->transform().data());
    glColor3ubv(&_color.r);

    if (hasTexture()) {
        glEnable(GL_TEXTURE_2D);

        if (hasCheckerTexture) {
            if (!hasCreatedCheckerTexture) {
                hasCreatedCheckerTexture = true;
            }
            CheckerTexture(hasCreatedCheckerTexture);
        }
        else {
            if (hasCreatedCheckerTexture) {
                hasCreatedCheckerTexture = false;
            }
            _texture.bind();
        }
    }

    if (hasMesh()) _mesh_ptr->draw();

    if (hasTexture()) glDisable(GL_TEXTURE_2D);

    // Dibuja a los hijos recursivamente desde aqu�
    for (const auto& child : children()) {
        child->draw(); // Cada hijo se dibuja relativo a su padre
        drawBoundingBox(child->boundingBox()); // Tambi�n dibujamos sus bounding boxes
    }

    glPopMatrix();
}

BoundingBox GameObject::localBoundingBox() const {
    if (children().size()) {
        BoundingBox bbox = _mesh_ptr ? _mesh_ptr->boundingBox() : children().front()->boundingBox();
        for (const auto& child : children()) bbox = bbox + child->boundingBox();
        return bbox;
    }
    else return _mesh_ptr ? _mesh_ptr->boundingBox() : BoundingBox();
}

BoundingBox GameObject::worldBoundingBox() const {
    BoundingBox bbox = worldTransform().mat() * (_mesh_ptr ? _mesh_ptr->boundingBox() : BoundingBox());
    for (const auto& child : children()) bbox = bbox + child->worldBoundingBox();
    return bbox;
}

std::string GameObject::GetName() const {
    return name;
}

void GameObject::SetName(const std::string& name) {
    this->name = name;
}

void GameObject::SetParent(std::shared_ptr<GameObject> newParent) {
    if (auto currentParent = GetParent()) {
        currentParent->removeChild(*this);
    }
    if (newParent) {
        newParent->addChild(shared_from_this());
    }
}

std::shared_ptr<GameObject> GameObject::GetParent() const {
    return parent().shared_from_this();
}

void GameObject::addChild(std::shared_ptr<GameObject> child) {
    emplaceChild(*child);
}

void GameObject::removeChild(GameObject& child) {
    auto it = std::find_if(children_.begin(), children_.end(), [&child](const std::shared_ptr<GameObject>& ptr) {
        return ptr.get() == &child;
        });
    if (it != children_.end()) {
        children_.erase(it);
    }
}

const std::vector<std::shared_ptr<GameObject>>& GameObject::children() const {
    return children_;
}

void GameObject::initializeCheckerTexture() {
    hasCheckerTexture = true;
    hasCreatedCheckerTexture = false;
}