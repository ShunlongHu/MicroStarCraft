//
// Created by zoe50 on 2023/12/9.
//

#include "client_interface.h"
#include <vector>
#include <iostream>

using namespace std;
static vector<signed char> observationVec1;
static vector<signed char> observationVec2;

extern "C" __declspec(dllexport) void Init(InitParam initParam) {
    cout << "Init " << initParam.w << "x" << initParam.h << endl;
    observationVec1.resize(initParam.h * initParam.w);
    for (int i = 0; i < observationVec1.size(); ++i) {
        observationVec1[i] = i;
    }
    observationVec2.resize(initParam.h * initParam.w);
    for (int i = 0; i < observationVec2.size(); ++i) {
        observationVec2[i] = -i;
    }
}

extern "C" __declspec(dllexport) TotalObservation Step(TotalAction totalAction) {
    return TotalObservation{Observation{observationVec1.data(), static_cast<int>(observationVec1.size())},
                            Observation{observationVec2.data(), static_cast<int>(observationVec2.size())}};
}