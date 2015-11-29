#include "QTree.h"
#include "RenderableThing.h"

QTree::Node::Node(const Rect &rec, Node *p, Count c):
    area(rec), parent(p), count(c)	//构造出来的节点默认都是叶子
{
	sub_area[lu] = sub_area[ru] = sub_area[rd] = sub_area[ld] = nullptr;
}

QTree::Node::~Node()
{
	parent = nullptr;
	delete sub_area[lu]; 
	delete sub_area[ru];
	delete sub_area[rd];
	delete sub_area[ld];

    for(int it = entitys.size() - 1; it >= 0; --it)
    {
        delete entitys[it];
    }
}

QTree::QTree(const Rect &rec)	//四叉树需要地图大小（矩形）来初始化
{
	root = new Node(rec, nullptr);
}

QTree::~QTree()
{
	delete root;
}

void QTree::Update()
{
    Update(root);
}

void QTree::Update(Node *r)
{
    if(r->is_leaf() == true)
    {
        for(int i = 0; i < r->entitys.size(); ++i)
        {
            if(r->entitys[i]->preArea.is_contain(*r->entitys[i]) == true)
            {
                continue;
            }
            else
            {
                shared_ptr<RenderableThing> *obj = static_cast<shared_ptr<RenderableThing>*>(r->entitys[i]->entity);
                remove(r->entitys[i], *r);
                r->entitys[i]->Update((*obj)->GetWorldPosition());
                insert(r->entitys[i]);
            }
        }
    }
    else
    {
        for(int dir = lu; dir <= ld; ++dir)
        {
            Update(r->sub_area[dir]);
        }
    }
}

void QTree::insert(QTreePos *pos)
{
	if(root->area.is_contain(*pos) == true)
	{
		insert(pos, *root);
	}
}

void QTree::insert(QTreePos *pos, Node &r)
{
	if(r.is_leaf() == false)	//若不是叶子，那么肯定不能插入，只能插入它其中一个子区域里面
	{
		for(int dir = lu; dir <= ld; ++dir)
		{
            if(r.sub_area[dir]->area.is_contain(*pos) == true)
			{
				insert(pos, *r.sub_area[dir]);
				break;
			}
		}
	}
    else	//叶子节点，把实体存入其中
    {
        r.entitys.push_back(pos);
        pos->preArea = r.area;
        if(r.is_full() == true)     //如果这个叶子节点存储满了
        {
            for(int dir = lu; dir <= ld; ++dir)
            {
                r.sub_area[dir] = new Node(cut_rect(r.area, static_cast<Direct>(dir)), &r);
                for(auto it = r.entitys.begin(); it != r.entitys.end(); )
                {
                    if(r.sub_area[dir]->area.is_contain(**it) == true)
                    {
                        insert(&**it, *r.sub_area[dir]);
                        it = r.entitys.erase(it);
                    }
                    else
                    {
                        ++it;
                    }
                }
            }
            r.entitys.clear();
        }
	}
}

void QTree::remove(QTreePos *pos)
{
	remove(pos, *root);
}

void QTree::remove(QTreePos *pos, Node &r)
{
    if(r.area.is_contain(*pos) == false)
    {
        return ;
    }
    auto it = r.find_entity(*pos);
    if(it != r.entitys.end())
	{
        r.erase_entity(it);
		adjust(r);
	}
	else if(r.is_leaf() == false)
	{
		for(int dir = lu; dir <= ld; ++dir)
		{
			if(r.sub_area[dir]->area.is_contain(*pos) == true)
			{
				remove(pos, *r.sub_area[dir]);
				break;
			}
		}
	}
}

std::vector<QTreePos> QTree::findInRect(const Rect &rt)
{
    std::vector<QTreePos> ret;
    find_in_node(*root, rt, ret);
    return ret;
}

void QTree::find_in_node(const Node &r, const Rect &rt, std::vector<QTreePos> &ret)
{
    if(r.area.is_intersect(rt) == true)
    {
        if(r.is_leaf() == true)
        {
            for(auto it = r.entitys.begin(); it != r.entitys.end(); ++it)
            {
                if(rt.is_contain(**it) == true)
                {
                    ret.push_back(**it);
                }
            }
        }
        else
        {
            for(int i = lu; i <= ld; ++i)
            {
                find_in_node(*r.sub_area[i], rt, ret);
            }
        }
    }
}

void QTree::adjust(Node &r)
{
    if(r.parent == nullptr || r.is_empty() != true)	//叶子节点还存储有实体或者已经降级到根了，不能再降
	{
		return ;
	}
    Node *p = r.parent;
	bool flag = true;
	for(int dir = lu; dir <= ld; ++dir)
	{
        if(p->sub_area[dir]->is_leaf() != true || p->sub_area[dir]->is_empty() != true)
		{
			flag = false;
			break;
		}
	}
	if(flag == true)		//中间节点降级为叶子节点
	{
		for(int dir = lu; dir <= ld; ++dir)
		{
			delete p->sub_area[dir];
			p->sub_area[dir] = nullptr;
		}
		adjust(*p);
	}
}