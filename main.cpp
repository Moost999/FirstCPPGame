#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <vector>

// Prototipos
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void SetupPixelFormat(HDC hdc);
void DrawScene();
void SetupOpenGL();
void UpdateGame();
void DrawCube();

// Variáveis globais
HINSTANCE hInst;
HWND hWnd;

// Variáveis de movimento
float playerZ = 0.0f;   // Posição do jogador (em Z)
float playerY = 0.0f;   // Altura do jogador
float velocityY = 0.0f; // Velocidade vertical (para pulo)
bool isJumping = false; // Verifica se o jogador está pulando
float gravity = -0.001f; // Gravidade para o pulo
float groundY = 0.0f;   // Nível do chão

// Blocos (obstáculos)
struct Block {
    float x, z;
};

std::vector<Block> blocks = {{0.0f, -5.0f}, {0.0f, -10.0f}, {0.0f, -15.0f}}; // Posição dos blocos

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    hInst = hInstance;

    // Criação da janela
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "OpenGLWindowClass";
    RegisterClass(&wc);

    hWnd = CreateWindow("OpenGLWindowClass", "Runner 3D", WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, NULL);
    
    ShowWindow(hWnd, nCmdShow);

    // Inicializa OpenGL
    HDC hdc = GetDC(hWnd);
    SetupPixelFormat(hdc);
    HGLRC hglrc = wglCreateContext(hdc);
    wglMakeCurrent(hdc, hglrc);

    // Configurar OpenGL
    SetupOpenGL();

    // Loop de mensagem
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        // Atualizar lógica do jogo
        UpdateGame();

        // Redesenha a cena
        InvalidateRect(hWnd, NULL, FALSE);
    }

    // Limpeza
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hglrc);
    ReleaseDC(hWnd, hdc);
    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_PAINT:
            DrawScene();
            ValidateRect(hwnd, NULL);
            break;
        case WM_KEYDOWN:
            if (wParam == VK_SPACE && !isJumping) {
                isJumping = true;
                velocityY = 0.02f; // Velocidade inicial do pulo
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

void SetupPixelFormat(HDC hdc) {
    PIXELFORMATDESCRIPTOR pfd = {0};
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int pixelFormat = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pixelFormat, &pfd);
}

void SetupOpenGL() {
    glEnable(GL_DEPTH_TEST); // Ativa o teste de profundidade

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, 800.0f / 600.0f, 0.1f, 100.0f); // Configura a perspectiva
    glMatrixMode(GL_MODELVIEW);
}

void UpdateGame() {
    // Atualiza a posição do jogador
    playerZ -= 0.05f; // O jogador sempre se move para frente (em Z)

    // Atualiza a física do pulo
    if (isJumping) {
        playerY += velocityY;
        velocityY += gravity;

        // Verifica se o jogador voltou ao chão
        if (playerY <= groundY) {
            playerY = groundY;
            isJumping = false;
        }
    }

    // Verificar colisão com blocos
    for (auto& block : blocks) {
        if (block.z > playerZ - 0.5f && block.z < playerZ + 0.5f && playerY <= 0.5f) {
            // Colidiu com o bloco (reinicia o jogo, por exemplo)
            playerZ = 0.0f;
        }
    }
}

void DrawCube(float size) {
    float halfSize = size / 2.0f;

    glBegin(GL_QUADS);
    
    // Frente
    glColor3f(1.0f, 0.0f, 0.0f); // vermelho
    glVertex3f(-halfSize, -halfSize, halfSize);
    glVertex3f(halfSize, -halfSize, halfSize);
    glVertex3f(halfSize, halfSize, halfSize);
    glVertex3f(-halfSize, halfSize, halfSize);

    // Trás
    glColor3f(0.0f, 1.0f, 0.0f); // verde
    glVertex3f(-halfSize, -halfSize, -halfSize);
    glVertex3f(-halfSize, halfSize, -halfSize);
    glVertex3f(halfSize, halfSize, -halfSize);
    glVertex3f(halfSize, -halfSize, -halfSize);

    // Esquerda
    glColor3f(0.0f, 0.0f, 1.0f); // azul
    glVertex3f(-halfSize, -halfSize, -halfSize);
    glVertex3f(-halfSize, -halfSize, halfSize);
    glVertex3f(-halfSize, halfSize, halfSize);
    glVertex3f(-halfSize, halfSize, -halfSize);

    // Direita
    glColor3f(1.0f, 1.0f, 0.0f); // amarelo
    glVertex3f(halfSize, -halfSize, -halfSize);
    glVertex3f(halfSize, halfSize, -halfSize);
    glVertex3f(halfSize, halfSize, halfSize);
    glVertex3f(halfSize, -halfSize, halfSize);

    // Topo
    glColor3f(1.0f, 0.0f, 1.0f); // rosa
    glVertex3f(-halfSize, halfSize, halfSize);
    glVertex3f(halfSize, halfSize, halfSize);
    glVertex3f(halfSize, halfSize, -halfSize);
    glVertex3f(-halfSize, halfSize, -halfSize);

    // Base
    glColor3f(0.0f, 1.0f, 1.0f); // ciano
    glVertex3f(-halfSize, -halfSize, halfSize);
    glVertex3f(-halfSize, -halfSize, -halfSize);
    glVertex3f(halfSize, -halfSize, -halfSize);
    glVertex3f(halfSize, -halfSize, halfSize);

    glEnd();
}

void DrawScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Configura a câmera
    gluLookAt(0.0f, 2.0f, 5.0f, 0.0f, 0.0f, playerZ, 0.0f, 1.0f, 0.0f);

    // Desenhar o jogador (um cubo)
    glPushMatrix();
    glTranslatef(0.0f, playerY + 0.5f, playerZ); // Move o jogador para sua posição
    glColor3f(0.0f, 1.0f, 0.0f); // Cor verde
    glPopMatrix();
    DrawCube(2.0f);

    // Desenhar blocos (obstáculos)
    for (const auto& block : blocks) {
        glPushMatrix();
        glTranslatef(block.x, 0.5f, block.z); // Move o bloco para sua posição
        glColor3f(3.0f, 2.0f, 1.0f); // Cor vermelha
        glPopMatrix();
    }

    // Troca de buffers
    HDC hdc = GetDC(hWnd);
    SwapBuffers(hdc);
    ReleaseDC(hWnd, hdc);
}
