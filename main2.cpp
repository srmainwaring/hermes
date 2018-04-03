//
// Created by frongere on 03/04/18.
//

#include <memory>
#include <vector>
#include <iostream>

struct MyDT {
    int i = 1;
    double d = 2;
};



template <class T>
class AbstractNode;

class Container;


class Visitor {

public:

    void visit(const AbstractNode<int>& top) const;

    void visit(const AbstractNode<double>& top) const;

    void visit(const AbstractNode<MyDT>& top) const;

    void visit(const AbstractNode<Container>& cont) const;


};



class Visitable {  // Visitable

public:
    virtual void Accept(const Visitor& visitor) const = 0;


};


template <class T>
class AbstractNode : public Visitable {

private:
    T* m_data = nullptr;

public:
    AbstractNode() {}
    explicit AbstractNode(T* data) : m_data(data) {}


    void Accept(const Visitor& visitor) const override {
        visitor.visit(*this);
    }

    const T* GetData() const {
        return m_data;
    }


};

class IntNode : public AbstractNode<IntNode> {};



class Container {
private:
    typedef std::vector<std::unique_ptr<Visitable>> VectorType;
    VectorType m_vector;

public:
    template <class T>
    void Add(T* val) {
        m_vector.emplace_back(std::make_unique<AbstractNode<T>>(val));
    }

    void visit(const Visitor& visitor) const {
        for (const auto& e : m_vector) {
            e->Accept(visitor);
        }
    }


    typedef VectorType::iterator iterator;
    iterator begin() { return m_vector.begin(); }
    iterator end() { return m_vector.end(); }
    typedef VectorType::const_iterator const_iterator;
    const_iterator cbegin() { return m_vector.cbegin(); }
    const_iterator cend() { return m_vector.cend(); }

};

void Visitor::visit(const AbstractNode<int> &top) const {
    std::cout << "I am integer with value " << *top.GetData() << std::endl;
}

void Visitor::visit(const AbstractNode<double> &top) const {
    std::cout << "I am double with value " << *top.GetData() << std::endl;
}

void Visitor::visit(const AbstractNode<MyDT> &top) const {
    std::cout << "I am DT with val "<< top.GetData()->i << " and " << top.GetData()->d << "\n";
}

void Visitor::visit(const AbstractNode<Container> &cont) const {
    std::cout << "I am a container and I am going to visit myself\n";
    cont.GetData()->visit(*this);

}



int main() {

    int i = 2;
    double d = 3.148769876986;
    MyDT dt;


    Container c;

    c.Add<int>(&i);
    c.Add<double>(&d);

    c.Add(&dt);

//    c.Print();
    Visitor visitor;


    c.visit(visitor);



    Container c2;
    int i2 = 10;
    double d2 = 1.414;
    c2.Add<int>(&i2);
    c2.Add<double>(&d2);

    c.Add<Container>(&c2);

    c.visit(visitor);


//    for (const auto& e : c) {
//        e->Accept(visitor);
//    }








    return 0;
}