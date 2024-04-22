#pragma once

#include "common.h"

namespace Tactical {

namespace DecisionTree {

#define READBUFFERSIZE   512
#define CSV
//#define METHOD_C45   //划分方式,c4.5

//参与训练的属性起始列
//参与训练的属性结束列
//标签编号编号从0开始
////tacticalpalnner
#define TRAINATTR_STR    0
#define TRAINATTR_ED     3
#define LABEL_LOC        4

//训练集路径
//预测集路径
//序列化文件路径

#define  TRAINDATAS_ADDR     "E:/data/tacticaldata/longitudinal/blockstate.csv"
#define  PREDICTDATAS_ADDR   "E:/data/tacticaldata/longitudinal/blockstate.csv"
#define  EXPORTTREE_ADDR     "E:/data/tacticaldata/longitudinal/blockstateTree.csv"

 enum class ERRORDTree
{
   NOT_ERROR,
   ERROR_FREAD,
   ERROR_FWRITE,
   ERROR_FINDVALUE,

   ERROR_PARA,//输入的参数错误

   ERROR_TRAIN


};

struct Node
{
    std::string attribute;
    std::string edgeValue;
    std::vector<Node*> childs;
    Node() : attribute(""), edgeValue("") { }
};

template <typename T>
using Ptr = T*;
using TreeRoot = Ptr<Node>;

using Ttacticaldata = std::pair<std::vector<std::string>,std::vector<std::string >>; //单个样本数据
using Ttacticaldatas = std::vector<Ttacticaldata>; //训练集
using Tattributes = std::vector<std::string>; //所有属性集
using Tmap_attributes = std::map<std::string, std::vector<std::string> >;//所有属性集的映射关系
using Tlabel = std::map<std::string, std::vector<std::string>>;//标签



}//DecisionTree
}//Tactical



