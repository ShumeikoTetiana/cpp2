#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

template <typename T>
struct Node {
    T data;
    std::shared_ptr<Node<T>> next;
    std::weak_ptr<Node<T>> prev;
    Node(const T& data) : data(data), next(nullptr), prev({}) {}
};

template <typename T>
class LinkedList {
private:
    std::shared_ptr<Node<T>> head;

public:
    LinkedList() : head(nullptr) {}

    void push_front(const T& data) {
        auto newNode = std::make_shared<Node<T>>(data);
        newNode->next = head;
        head = newNode;
    }

    void push_back(const T& data) {
        auto newNode = std::make_shared<Node<T>>(data);
        if (!head) {
            head = newNode;
            return;
        }
        std::shared_ptr<Node<T>> current = head;
        while (current->next) {
            current = current->next;
        }
        current->next = newNode;
    }

    std::shared_ptr<T> pop_front() {
        if (!head) {
            throw std::out_of_range("List is empty");
        }
        std::shared_ptr<T> data = std::make_shared<T>(head->data);
        head = head->next;
        return data;
    }

    std::shared_ptr<T> pop_back() {
        if (!head) {
            throw std::out_of_range("List is empty");
        }
        if (!head->next) {
            return pop_front();
        }
        std::shared_ptr<Node<T>> current = head;
        std::shared_ptr<Node<T>> prev = nullptr;
        while (current->next) {
            prev = current;
            current = current->next;
        }
        std::shared_ptr<T> data = std::make_shared<T>(current->data);
        if (prev) {
            prev->next = nullptr;
        }
        return data;
    }

    std::shared_ptr<T> get(size_t index) const {
        std::shared_ptr<Node<T>> current = head;
        for (size_t i = 0; i < index; ++i) {
            if (!current) {
                throw std::out_of_range("Index out ");
            }
            current = current->next;
        }
        if (!current) {
            throw std::out_of_range("Index out ");
        }
        return std::make_shared<T>(current->data);
    }

    void insert(size_t index, const T& data) {
        if (index == 0) {
            push_front(data);
            return;
        }
        auto newNode = std::make_shared<Node<T>>(data);
        std::shared_ptr<Node<T>> current = head;
        std::shared_ptr<Node<T>> prev = nullptr;
        for (size_t i = 0; i < index; ++i) {
            if (!current) {
                throw std::out_of_range("Index out ");
            }
            prev = current;
            current = current->next;
        }
        if (!prev) {
            throw std::out_of_range("Index out ");
        }
        newNode->next = current;
        prev->next = newNode;
    }

    std::shared_ptr<T> remove(size_t index) {
        if (!head) {
            throw std::out_of_range("List is empty");
        }
        if (index == 0) {
            return pop_front();
        }
        std::shared_ptr<Node<T>> current = head;
        std::shared_ptr<Node<T>> prev = nullptr;
        for (size_t i = 0; i < index; ++i) {
            if (!current) {
                throw std::out_of_range("Index out ");
            }
            prev = current;
            current = current->next;
        }
        if (!current) {
            throw std::out_of_range("Index out ");
        }
        std::shared_ptr<T> data = std::make_shared<T>(current->data);
        if (prev) {
            prev->next = current->next;
        }
        return data;
    }

    size_t size() const {
        size_t count = 0;
        std::shared_ptr<Node<T>> current = head;
        while (current) {
            count++;
            current = current->next;
        }
        return count;
    }

    bool empty() const {
        return head == nullptr;
    }

    std::shared_ptr<size_t> find(const T& data) const {
        std::shared_ptr<Node<T>> current = head;
        size_t index = 0;
        while (current) {
            if (current->data == data) {
                return std::make_shared<size_t>(index);
            }
            current = current->next;
            index++;
        }
        return nullptr;
    }

    friend std::ostream& operator<<(std::ostream& os, const LinkedList& list) {
        std::shared_ptr<Node<T>> current = list.head;
        os << "[";
        while (current) {
            os << current->data;
            if (current->next) {
                os << ", ";
            }
            current = current->next;
        }
        os << "]";
        return os;
    }
};

template <typename T>
class DoubleLinkedList {
private:
    std::shared_ptr<Node<T>> head;
    std::weak_ptr<Node<T>> tail;

public:
    DoubleLinkedList() : head(nullptr), tail({}) {}

    void push_front(const T& data) {
        auto newNode = std::make_shared<Node<T>>(data);
        newNode->next = head;
        if (head) {
            head->prev = newNode;
        }
        head = newNode;
        if (tail.expired()) {
            tail = head;
        }
    }

    void push_back(const T& data) {
        auto newNode = std::make_shared<Node<T>>(data);
        auto sharedTail = tail.lock();
        newNode->prev = sharedTail;
        if (sharedTail) {
            sharedTail->next = newNode;
        }
        tail = newNode;
        if (!head) {
            head = newNode;
        }
    }

    std::shared_ptr<T> pop_front() {
        if (!head) {
            throw std::out_of_range("List is empty");
        }
        std::shared_ptr<T> data = std::make_shared<T>(head->data);
        head = head->next;
        if (head) {
            head->prev = {};
        } else {
            tail = {};
        }
        return data;
    }

    std::shared_ptr<T> pop_back() {
        auto sharedTail = tail.lock();
        if (!sharedTail) {
            throw std::out_of_range("List is empty");
        }
        std::shared_ptr<T> data = std::make_shared<T>(sharedTail->data);
        tail = sharedTail->prev;
        if (auto prevNode = sharedTail->prev.lock()) {
            prevNode->next = nullptr;
        } else {
            head = nullptr;
        }
        return data;
    }

    std::shared_ptr<T> get(size_t index) const {
        auto current = head;
        for (size_t i = 0; i < index; ++i) {
            if (!current) {
                throw std::out_of_range("Index out ");
            }
            current = current->next;
        }
        if (!current) {
            throw std::out_of_range("Index out ");
        }
        return std::make_shared<T>(current->data);
    }

    void insert(size_t index, const T& data) {
        if (index == 0) {
            push_front(data);
            return;
        }
        auto newNode = std::make_shared<Node<T>>(data);
        auto current = head;
        for (size_t i = 0; i < index; ++i) {
            if (!current) {
                throw std::out_of_range("Index out ");
            }
            current = current->next;
        }
        if (!current) {
            push_back(data);
            return;
        }
        newNode->prev = current->prev;
        newNode->next = current;
        if (auto prevNode = current->prev.lock()) {
            prevNode->next = newNode;
        }
        current->prev = newNode;
    }

    std::shared_ptr<T> remove(size_t index) {
        if (!head) {
            throw std::out_of_range("List is empty");
        }
        if (index == 0) {
            return pop_front();
        }
        auto current = head;
        for (size_t i = 0; i < index; ++i) {
            if (!current) {
                throw std::out_of_range("Index out ");
            }
            current = current->next;
        }
        if (!current) {
            throw std::out_of_range("Index out ");


        }
        std::shared_ptr<T> data = std::make_shared<T>(current->data);
        auto prevNode = current->prev.lock();
        auto nextNode = current->next;
        if (prevNode) {
            prevNode->next = nextNode;
        }
        if (nextNode) {
            nextNode->prev = prevNode;
        } else {
            tail = prevNode;
        }
        return data;
    }

    size_t size() const {
        size_t count = 0;
        auto current = head;
        while (current) {
            count++;
            current = current->next;
        }
        return count;
    }

    bool empty() const {
        return head == nullptr;
    }

    std::shared_ptr<size_t> find(const T& data) const {
        auto current = head;
        size_t index = 0;
        while (current) {
            if (current->data == data) {
                return std::make_shared<size_t>(index);
            }
            current = current->next;
            index++;
        }
        return nullptr;
    }

    friend std::ostream& operator<<(std::ostream& os, const DoubleLinkedList& list) {
        auto current = list.head;
        os << "[";
        while (current) {
            os << current->data;
            if (current->next) {
                os << ", ";
            }
            current = current->next;
        }
        os << "]";
        return os;
    }
};

int main() {
    LinkedList<int> sll;
    sll.push_front(3);
    sll.push_back(5);
    sll.push_front(1);
    std::cout << "Singly Linked List: " << sll << std::endl;
    std::cout << "Size: " << sll.size() << std::endl;
    std::cout << "Element at index 0: " << *sll.get(0) << std::endl;

    sll.insert(2, 4);

    std::cout << "Singly Linked List after insertion: " << sll << std::endl;

    sll.remove(1);

    std::cout << "Singly Linked List after removal: " << sll << std::endl;
    if (auto index = sll.find(4)) {
        std::cout << "Element 4 found at index: " << *index << std::endl;
    }
    try {
        std::cout << "Popped front element: " << *sll.pop_front() << std::endl;
        std::cout << "Singly Linked List after pop_front: " << sll << std::endl;
        std::cout << "Popped back element: " << *sll.pop_back() << std::endl;
        std::cout << "Singly Linked List after pop_back: " << sll << std::endl;

    } catch (const std::out_of_range& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    std::cout << "\n";

    DoubleLinkedList<std::string> dll;
    dll.push_front("banana");
    dll.push_back("apple");
    dll.push_front("cherry");
    std::cout << "Doubly Linked List: " << dll << std::endl;

    return 0;
}