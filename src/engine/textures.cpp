#include "main.h"
#include "textures.h"

using namespace std;

TextureManager& TextureManager::getInstance()
{
    static TextureManager instance;
    return instance;
}

bool TextureManager::load(const string& fileName, const string& id, SDL_Renderer* pRenderer) // load a texture
{
    SDL_Surface* pTempSurface = IMG_Load(fileName.c_str());
    if(pTempSurface == nullptr)
    {
        logoutf("warning: could not load texture (%s)", fileName.c_str());
        return false;
    }

    SDL_Texture* pTexture = SDL_CreateTextureFromSurface(pRenderer, pTempSurface);
    SDL_FreeSurface(pTempSurface);

    if(pTexture != nullptr)
    {
        m_textureMap[id] = pTexture;
        SDL_SetTextureBlendMode(pTexture, SDL_BLENDMODE_BLEND);
        return true;
    }
    return false;
}

void TextureManager::preloadTextures() // preload images used in the game, soft-coded in textures.cfg (except font)
{
    TextureManager& textureManager = TextureManager::getInstance();

    if(!textureManager.load("data/gui/font_1.png", "MainFont", renderer) || !textureManager.load("data/gui/font_2.png", "DialFont", renderer))
        fatal("Unable to load font texture!");

    ifstream configFile("config/textures.cfg");
    if (!configFile.is_open()) fatal("Unable to open textures.cfg!");

    string line;
    while (getline(configFile, line))
    {
        istringstream iss(line);
        string command, filePath, textureID;

        if (iss >> command >> filePath >> textureID)
        {
            if (command == "texture") textureManager.load(filePath, textureID, renderer);
        }
    }
    configFile.close();
}

void TextureManager::draw(const string& id, int x, int y, int width, int height, SDL_Renderer* pRenderer, SDL_RendererFlip flip) //draw an entire texture
{
    SDL_Rect srcRect;
    SDL_Rect destRect;

    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = destRect.w = width;
    srcRect.h = destRect.h = height;
    destRect.x = x;
    destRect.y = y;

    SDL_RenderCopyEx(pRenderer, m_textureMap[id], &srcRect, &destRect, 0, nullptr, flip);
}

void TextureManager::drawShadowedTex(const std::string& textureID, int x, int y, int width, int height, SDL_Renderer* renderer, uint32_t originalColor, uint32_t shadowColor, int offsetX, int offsetY, Uint8 shadowAlpha)
{
    TextureManager& textureManager = TextureManager::getInstance();
    // shadow
    textureManager.setColorMod(textureID, shadowColor);
    textureManager.setAlpha(textureID, shadowAlpha);
    textureManager.draw(textureID, x+offsetX, y+offsetY, width, height, renderer);
    // original
    textureManager.setColorMod(textureID, originalColor);
    textureManager.setAlpha(textureID, 255);
    textureManager.draw(textureID, x, y, width, height, renderer);

    textureManager.setColorMod(textureID, 0xFFFFFF);
}

void TextureManager::drawAlphaTex(const std::string& textureID, int x, int y, int width, int height, SDL_Renderer* renderer, Uint8 alphaBlend)
{
    TextureManager& textureManager = TextureManager::getInstance();

    textureManager.setAlpha(textureID, alphaBlend);
    textureManager.draw(textureID, x, y, width, height, renderer);

    textureManager.setAlpha(textureID, 255);
}

void TextureManager::drawFrame(const string& textureID, int x, int y, int width, int height, int srcX, int srcY, int srcW, int srcH, int scale, SDL_Renderer* renderer) // draw choosen part of a texture
{
    SDL_Rect srcRect = {srcX, srcY, srcW, srcH};
    SDL_Rect destRect = {x, y, width * scale, height * scale};
    SDL_RenderCopy(renderer, m_textureMap[textureID], &srcRect, &destRect);
}

void TextureManager::setColorMod(const string& textureID, uint32_t colorMod) // change color of a texture
{
    auto it = m_textureMap.find(textureID);
    if(it != m_textureMap.end()) SDL_SetTextureColorMod(it->second, (colorMod >> 16) & 0xFF, (colorMod >> 8) & 0xFF, colorMod & 0xFF);
    else logoutf("warning: texture not found (%d)", textureID);
}

void TextureManager::setAlpha(const std::string& textureID, Uint8 alpha) // alpha mod for texture
{
    auto it = m_textureMap.find(textureID);
    if (it != m_textureMap.end()) SDL_SetTextureAlphaMod(it->second, alpha);
    else logoutf("warning: failed to set alpha, texture not found (%d)", textureID);
}

void TextureManager::clearTextures() // free all textures ressources before quitting
{
    for(auto& texture : m_textureMap) SDL_DestroyTexture(texture.second);
    m_textureMap.clear();
}
