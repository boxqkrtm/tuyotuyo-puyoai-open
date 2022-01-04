if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra -O3 -g")
#set(CMAKE_CXX_FLAGS_DEBUG "-g")
#set(CMAKE_CXX_FLAGS_RELEASE "-O3")
SET(SRCS tuyotuyo/Tuyotuyo.cpp tuyotuyo/DuelPy.cpp tuyotuyo/Simulator.cpp tuyotuyo/DlAI.cpp tuyotuyo/DuelAIAI.cpp tuyotuyo/Duel.cpp tuyotuyo/Duel.cpp tuyotuyo/Duel.cpp tuyotuyo/Duel.cpp tuyotuyo/Duel.cpp tuyotuyo/Duel.cpp tuyotuyo/Duel.cpp tuyotuyo/PuyoTsumo.cpp tuyotuyo/GameInfo.cpp tuyotuyo/CrossFunc.cpp tuyotuyo/FormAI.cpp tuyotuyo/SaveAI.cpp tuyotuyo/MSVAI.cpp tuyotuyo/AI.cpp tuyotuyo/Detector.cpp tuyotuyo/Detected.cpp tuyotuyo/Plan.cpp tuyotuyo/NeuralNet.cpp tuyotuyo/DuelToko.cpp tuyotuyo/PlanTest.cpp tuyotuyo/DuelOptimizer.cpp tuyotuyo/NeuralNet.cpp)

ADD_LIBRARY(tuyotuyo SHARED ${SRCS})

ADD_EXECUTABLE(tuyotuyo.out ${SRCS})
