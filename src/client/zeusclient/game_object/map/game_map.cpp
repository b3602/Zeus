#include "game_map.h"
#include <math.h>
#include "control\pool\picture_pool.h"

GameMap::GameMap()
{

    m_viewportPos.x = 0;
    m_viewportPos.y = 0;
    m_previousPos.x = 0;
    m_previousPos.y = 0;
}
bool GameMap::Load(string mapTex, string collisionMapTex)
{
    m_mapTex = PicturePool::Instance()->Get(mapTex);
    m_collisionMapTex = PicturePool::Instance()->Get(collisionMapTex);
    if (!m_mapTex)
    {
        return false;
    }
    if (!m_collisionMapTex)
    {
        return false;
    }
    return true;
}
GameMap::~GameMap()
{
    for (int i = 0; i < (int)m_coveringTex.size(); i++)
    {
        delete m_coveringTex[i];
    }
}
bool GameMap::SetViewport(float x, float y, float width, float height, roleVector rolePos)
{
    if (!m_mapTex)
    {
        return false;
    }
    if (x < 0 || y < 0 || width < 0 || height < 0 || x + width > m_mapTex->GetWidth() || y + height > m_mapTex->GetHeight() || 
        rolePos.x < 0 || rolePos.y < 0 || rolePos.x > width || rolePos.y > height)
    {
        return false;
    }
    m_previousPos = rolePos;
    m_viewportPos.x = x;
    m_viewportPos.y = y;
    m_viewportWidth = width;
    m_viewportHeight = height;
    return true;
}

void GameMap::Render(roleVector rolePos)
{
    if (!m_mapTex)
    {
        return;
    }
    m_viewportPos.x = rolePos.x - m_viewportWidth / 2;
    if (m_viewportPos.x <  0)
        m_viewportPos.x = 0;

    if (m_viewportPos.x > m_mapTex->GetWidth() - m_viewportWidth)
        m_viewportPos.x =  m_mapTex->GetWidth() - m_viewportWidth;

    m_viewportPos.y = rolePos.y - m_viewportHeight / 2;
    if (m_viewportPos.y <  0)
        m_viewportPos.y = 0;

    if (m_viewportPos.y > m_mapTex->GetHeight() - m_viewportHeight)
        m_viewportPos.y = m_mapTex->GetHeight() - m_viewportHeight;
    
    m_mapTex->SetRenderRect(m_viewportPos.x, m_viewportPos.y, m_viewportWidth, m_viewportHeight);
    m_mapTex->Render(0, 0);
}

///绘制遮盖角色的部分 应该在角色绘制之后调用
void GameMap::RenderCovering()
{
    for (int i = 0; i < (int)m_coveringTex.size(); i++)
    {
        m_coveringTex[i]->Render(m_viewportPos);
    }
}

///添加遮掩物
bool GameMap::PushCovering(string path, float x, float y)
{
    CoveringTex* tex = new CoveringTex;
    if (!tex->Load(path, x, y))
    {
        delete tex;
        return false;
    }
    m_coveringTex.push_back(tex);
    return true;

}

///移除遮掩物
void GameMap::PopCovering()
{
    if (m_coveringTex.size() > 0)
    {
        m_coveringTex.pop_back();
    }
}

    
void GameMap::PopCoveringAll()
{
    if (m_coveringTex.size() > 0)
    {
        m_coveringTex.clear();
    }
}

bool GameMap::isCollision(roleVector nextPos, 
                          int r, ///单位所占面积的半径
                          DWORD color ///碰撞的颜色
                          )
{
    if (!m_mapTex)
    {
        return false;
    }
    if (!m_collisionMapTex)
    {
        return false;
    }
    for (int i = 0; i < ACCURACY; i++)
    {
        DWORD* pClr = m_collisionMapTex->CheckColor(
            nextPos.x + sin(2 * PI * i / ACCURACY) * r, 
            nextPos.y + cos(2 * PI * i / ACCURACY) * r, 1, 1);
        if (!pClr)
        {
            return true;
        }
        if (*pClr == color)
        {
            return true;
        }
    }
    return false;
}