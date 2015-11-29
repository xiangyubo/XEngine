#ifndef __QTREE__
#define __QTREE__

/*************************************
简易四叉树
除叶子节点外，每个节点都有四个孩子。
只有叶子节点能包含实体节点（坐标）。
不存在这样的中间节点，其四个孩子都是没有保存实体的叶子节点。
此处的实体节点是ecs中真正实体的坐标信息。
实现基本操作插入和删除。
entity移动时候利用这两个基本操作实现位置在树中的更新。
**************************************/
#include "PreHeader.h"
#include "Geometry.h"

struct QTreePos : public Position
{
    Rect    preArea;
    void    *entity;

    QTreePos(float _x = 0.0f, float _y = 0.0f, void *e = nullptr):Position(_x, _y),
        entity(e)
    {

    }

    QTreePos(Position pos, void *e = nullptr):Position(pos),
        entity(e)
    {

    }

    bool operator == (const QTreePos &pos) const
    {
        return x == pos.x && y == pos.y;
    }

    bool IsInPreArea()
    {
        return preArea.is_contain(*this);
    }

    void Update(Position pos)
    {
        x = pos.x;
        y = pos.y;
    }
};

class QTree         //四叉树类
{
private:
    enum Direct
    {
        lu = 0,
        ru = 1,
        rd = 2,
        ld = 3
    };

    struct Node
    {
        typedef std::vector<QTreePos*> Entities;
        typedef std::size_t Count;

        Count count;
        Rect area;
        Entities entitys;   //实体的坐标，会随着实体移动，所以只是持有一个坐标的指针，不拷贝，不析构
        Node * sub_area[4];
        Node * parent;		//指向父亲的指针，调节的时候会用到

        Node(const Rect &, Node *, Count c = 4);
        ~Node();

        //辅助函数，显示标记为内联函数
        bool is_leaf() const
        {
            return sub_area[lu] == nullptr &&
                sub_area[ru] == nullptr &&
                sub_area[rd] == nullptr &&
                sub_area[ld] == nullptr;
        }

        bool is_full() const
        {
            return count < entitys.size();
        }

        bool is_empty() const
        {
            return entitys.size() == 0;
        }

        Entities::iterator find_entity(const QTreePos &pos)
        {
            for(auto it = entitys.begin(); it != entitys.end(); ++it)
            {
                if(pos == **it)
                {
                    return it;
                }
            }
            return entitys.end();
        }

        void erase_entity(const Entities::iterator &it)
        {
            entitys.erase(it);
            delete *it;
        }
    };

    Node *root;
public:
    QTree(const Rect &);
    ~QTree();

    //更新所有节点的坐标，可能会包含插入删除操作，一帧一次
    void Update();

    //插入一个节点
    void insert(QTreePos *);

    //删除一个节点
    void remove(QTreePos *);

    //返回找到的节点序列
    std::vector<QTreePos> findInRect(const Rect &rt);
private:
    void Update(Node *);

    void insert(QTreePos *, Node &);
    void remove(QTreePos *, Node &);
    void find_in_node(const Node &, const Rect &, std::vector<QTreePos> &);
    void adjust(Node &);	//删除操作可能出现四个叶子都没有保存实体，用此函数调整 

    Rect cut_rect(const Rect &rec, const Direct dir)
    {
        QTreePos center((rec.lu.x + rec.rd.x)/2, (rec.lu.y + rec.rd.y)/2);
        switch (dir)
        {
        case lu:
            return Rect(rec.lu, center);
        case ru:
            return Rect(QTreePos(center.x, rec.lu.y), QTreePos(rec.rd.x, center.y));
        case rd:
            return Rect(center, rec.rd);
        case ld:
            return Rect(QTreePos(rec.lu.x, center.y), QTreePos(center.x, rec.rd.y));
        }
    }
};

#endif