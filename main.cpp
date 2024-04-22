#include <iostream>
#include<vector>
#include "tacticaldtree.h"
#include "TestSec/gentestdatas.h"
#include "dataprocessing/data_processing.h"
#include "stdlib.h"
#include <direct.h>
using namespace std;
using namespace Tactical::DecisionTree;
using namespace DTreeTest;
using namespace Tactical::Dataprocessing;
int main()
{
  //  CDataProcess CDataProcess;
 //  CDataProcess.deduplicate(SRCPATH,RM_DESPATH);
 // CDataProcess.removedata(RM_DESPATH,RM2_DESPATH,4,6);
 //   CDataProcess.removedata(RM2_DESPATH,RM3_DESPATH,7,8);
    int error = 0;
    CTacticalDTree TacticalDTree;
    ERRORDTree ERROR;

    //训练决策树
    ERROR=TacticalDTree.Train_Dtree(TacticalDTree.pTree,TRAINDATAS_ADDR);
#if 1
    //加载决策树
    TacticalDTree.Settreepath(std::string(STRVALUE(TREE_PATH_ROAD)) + "/TacticalplannerTree.csv");
    TacticalDTree.Settitlepath(std::string(STRVALUE(TREE_PATH_ROAD)) + "/tactical_attributes.csv");
    TacticalDTree.LOAD();

    //测试
    CTestDtree TestDtree;
    TestDtree.Test_load_file(TestDtree.predict_tacticaldatas, TestDtree.attributes,TestDtree.trainattributes,PREDICTDATAS_ADDR);


    for(auto predict_tacticaldata : TestDtree.predict_tacticaldatas)
    {

       TacticalDTree.predict_result = TacticalDTree.Predict(TacticalDTree.offl_pTree,predict_tacticaldata);

       if(TacticalDTree.predict_result.compare(predict_tacticaldata.second[LABEL_LOC]))
       {
#ifdef INCLUDE_QT
           qDebug() << "predict : "<<QString::fromStdString(TacticalDTree.predict_result);
           qDebug() << "real : "<<QString::fromStdString(predict_tacticaldata.second[LABEL_LOC]);
#endif
           error++;
       }
    }
#ifdef INCLUDE_QT
    qDebug()<<"->"<<error;
#endif
#endif
    return 0;
}
