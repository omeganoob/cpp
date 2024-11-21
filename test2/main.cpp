#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

// A simple class to demonstrate serialization
class Person {
private:
    std::string name;
    int age;
    std::vector<std::string> hobbies;
    
public:
    Person(const std::string& n = "", int a = 0) : name(n), age(a) {}
    
    void addHobby(const std::string& hobby) {
        hobbies.push_back(hobby);
    }
    
    // Serialize to string (custom format)
    std::string serialize() const {
        std::stringstream ss;
        ss << "BEGIN_PERSON\n";
        ss << "NAME=" << name << "\n";
        ss << "AGE=" << age << "\n";
        ss << "HOBBIES_COUNT=" << hobbies.size() << "\n";
        for (const auto& hobby : hobbies) {
            ss << "HOBBY=" << hobby << "\n";
        }
        ss << "END_PERSON";
        return ss.str();
    }
    
    // Deserialize from string
    static Person deserialize(const std::string& data) {
        Person person;
        std::stringstream ss(data);
        std::string line;
        
        while (std::getline(ss, line)) {
            if (line == "BEGIN_PERSON") continue;
            if (line == "END_PERSON") break;
            
            size_t pos = line.find('=');
            if (pos == std::string::npos) continue;
            
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            if (key == "NAME") {
                person.name = value;
            } else if (key == "AGE") {
                person.age = std::stoi(value);
            } else if (key == "HOBBY") {
                person.hobbies.push_back(value);
            }
        }
        
        return person;
    }
    
    // For demonstration
    void print() const {
        std::cout << "Name: " << name << "\n";
        std::cout << "Age: " << age << "\n";
        std::cout << "Hobbies:\n";
        for (const auto& hobby : hobbies) {
            std::cout << "- " << hobby << "\n";
        }
    }
};

// Example usage
int main() {
    // Create and populate a person
    Person person("John Doe", 30);
    person.addHobby("Reading");
    person.addHobby("Gaming");
    
    // Serialize
    std::string serialized = person.serialize();
    std::cout << "Serialized data:\n" << serialized << "\n\n";
    
    // Deserialize
    Person deserialized = Person::deserialize(serialized);
    std::cout << "Deserialized person:\n";
    deserialized.print();
    
    return 0;
}