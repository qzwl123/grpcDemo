

#include <iostream>
#include <string>
#include "./Person.pb.h"

using namespace std;



int main() {
    // 序列化
    Person p;
    p.set_id(10);
    p.set_age(32);
    p.set_sex("man");
    p.set_name("ttw");
    cout << "hellow" << endl;
    
    string output;
    p.SerializeToString(&output);
    
    Person pp;
    pp.ParseFromString(output);

    cout << pp.id() << " " << pp.age() << " " << pp.sex() << " " << pp.name() << endl;

    return 0;
}