#include "NodeModel.h"

NodeModel::NodeModel (const NodeModel& other)
    : id (other.id),
    dependentNodes (other.dependentNodes)
{
}

NodeModel& NodeModel::operator= (const NodeModel& other)
{
    this->id = ID (other.getID ());
    this->dependentNodes = other.dependentNodes;
    return *this;
}

bool operator==(const NodeModel& lhs, const NodeModel& rhs)
{
    if (lhs.getID ().getNumber () == rhs.getID ().getNumber ())
        return true;

    return false;
}

bool operator!=(const NodeModel& lhs, const NodeModel& rhs)
{
    if (lhs.getID ().getNumber () == rhs.getID ().getNumber ())
        return false;

    return true;
}

void NodeModel::addDependentNode (const NodeModel::ID id)
{
    dependentNodes.push_back (id);
}

void NodeModel::removeDependentNode (const NodeModel::ID id)
{
    auto iter = dependentNodes.cbegin ();

    while (iter != dependentNodes.cend ())
    {
        if (iter->getNumber () == id.getNumber ())
        {
            dependentNodes.erase (iter);
            break;
        }
        ++iter;
    }
}

void NodeModel::clearDependentNodes ()
{
    dependentNodes.clear ();
}

int NodeModel::dependentNodeCount () const
{
    return dependentNodes.size ();
}

const std::vector<NodeModel::ID>& NodeModel::getDependentNodes () const
{
    return dependentNodes;
}

const NodeModel NodeModel::Empty = NodeModel (-99);
