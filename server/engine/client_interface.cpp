//
// Created by zoe50 on 2023/12/9.
//

#include "client_interface.h"
#include <vector>
#include <iostream>

using namespace std;
static vector<signed char> observationVec1;
static vector<signed char> observationVec2;
static vector<GameState> gameStateVec;

extern "C" __declspec(dllexport) void Init(InitParam initParam) {
    cout << "Init " << initParam.w << "x" << initParam.h << " with " << initParam.numWorkers << " workers." << endl;
    gameStateVec.resize(initParam.numWorkers,{{}, initParam.w, initParam.h});

    auto observationSize = initParam.numWorkers * initParam.h * initParam.w * OBSERVATION_PLANE_NUM;

    observationVec1.resize(observationSize);
    for (int i = 0; i < observationVec1.size(); ++i) {
        observationVec1[i] = i;
    }
    observationVec2.resize(observationSize);
    for (int i = 0; i < observationVec2.size(); ++i) {
        observationVec2[i] = -i;
    }
}

extern "C" __declspec(dllexport) TotalObservation Step(TotalAction totalAction) {
    cout << totalAction.action1.size << ":";
    for (int i = 0; i < totalAction.action1.size; ++i) {
        cout << static_cast<int>(totalAction.action1.data[i]) << " ";
    }
    cout << endl;
    cout << totalAction.action2.size << ":";
    for (int i = 0; i < totalAction.action2.size; ++i) {
        cout << static_cast<int>(totalAction.action2.data[i]) << " ";
    }
    cout << endl;
    return TotalObservation{Observation{observationVec1.data(), static_cast<int>(observationVec1.size())},
                            Observation{observationVec2.data(), static_cast<int>(observationVec2.size())}};
}