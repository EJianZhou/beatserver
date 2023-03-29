#include <bits/stdc++.h>
#include <unistd.h>
#include <fcntl.h>
#include <gperftools/profiler.h>
#include <jemalloc/jemalloc.h>
using namespace std;

int a(int loop){
    int sum = loop;
    for(int i = 0; i < loop; i ++){
        sum += i;
    }
    return sum;
}

int b(int loop){
    int sum = loop;
    for(int i = 0; i < loop; i ++){
        sum += i;
    }
    return sum;
}

int main(){
    ProfilerStart("test.prof");
    for(int i = 0; i < 1000 * 1000; i ++){
        a(10);
        b(100);
    }
    int* p = new int(1);
    mallctl("prof.dump",NULL,NULL,NULL,0);    

    ProfilerStop();
}