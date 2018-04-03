//
// Created by frongere on 03/04/18.
//

#include <memory>
#include <vector>
#include <iostream>


template <class T>
class Top;


class VisitorBase {
private:

public:
    template <class T>
    void Apply(Top<T>& top) {
        std::cout << top.GetData() << "\n";
    }
};




//class VisitorGeneric {
//
//public:
//    template <class U>
//    void Apply(U* obj) {
//
//    }
//};




class Base {

public:
    virtual void Accept(VisitorBase& visitor) = 0;




};

template <class T>
class Coucou : public Base {

private:
    T* m_data;

public:
    explicit Coucou(T* data) : m_data(data) {}

    void Accept(VisitorBase& visitor) override {
//        visitor.Apply(this);
    }


};

class ContainerCoucou {
private:
    std::vector<std::unique_ptr<Base>> m_vector;

public:
    template <class T>
    void Add(T* val) {
        m_vector.emplace_back(std::make_unique<Coucou<T>>(val));
    }

};



template <class U>
class BaseInterface : public Base {

public:
//    void Interface() {
//        static_cast<U*>(this)->Implementation();
//    }

};



template <class T>
class Top : public BaseInterface<Top<T>> {

private:
    T* m_data;

public:

    explicit Top(T* val) : m_data(val) {};

    void Implementation() {
        std::cout << "Top";
    }

    T GetData() const {
        return *m_data;
    }

    void Accept(VisitorBase& visitor) override {
        visitor.Apply(*this);
    }
};



class Container {
private:
    typedef std::vector<std::unique_ptr<Base>> VectorType;
    VectorType m_vector;

public:
    template <class T>
    void Add(T* val) {
        m_vector.emplace_back(std::make_unique<Top<T>>(val));
    }

//    void Accept(VisitorBase& visitor) {
//        for (const auto& e : m_vector) {
//            visitor.Apply()
//        }
//    }



    void Print() {

        VisitorBase visitor;
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





int main() {

    int i = 2;
    double d = 3.148769876986;


    Container c;

    c.Add<int>(&i);
    c.Add<double>(&d);

    c.Print();

    for (const auto& e : c) {

    }



    ContainerCoucou c2;
    c2.Add<int>(&i);






    return 0;
}