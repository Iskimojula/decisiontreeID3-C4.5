#pragma once
#include "decisionstructure.h"
#include "dataprocessing/data_processing.h"
namespace Tactical {

namespace DecisionTree {

#define ImportTitle(name, datastrm)  ImportTree(name,datastrm);
class CTacticalDTree
{


public:
    CTacticalDTree(){};
    ~CTacticalDTree(){};

    TreeRoot pTree;//Node*
    TreeRoot offl_pTree;
//for predict

    std::string   predict_result;
    std::vector<std::string>   predict_attribute;
    std::string RESULT(std::vector<std::string> predictdata);
    std::string Predict(TreeRoot node,const Ttacticaldata apredict_tacticaldata);
    void Settreepath(const std::string path){
        treepath = path;
    }
    void Settitlepath(const std::string path){
       titlepath = path;
    }
//for train
    ERRORDTree Train_Dtree(TreeRoot &pTree,const std::string afilename);
//serialize

//deserialize
     ERRORDTree LOAD();


private:
        Dataprocessing::CDataProcess *dataprocess;
    //common
        template<typename T> void InitBuffer(T* addr,int size)
        {
            memset(addr,0x0,sizeof(T)*size);
        }

        template<typename T> inline T CalEntropy(T data)
        {
          return (data < 0.000001)? 0:data*log2(data);
        }
    ERRORDTree findattr_value(Ttacticaldata data, std::string attr,std::string &attr_value);

    //for train
    Ttacticaldata train_tacticaldata;
    Ttacticaldatas train_tacticaldatas;
    Tattributes  attributes;
    Tattributes  trainattributes;
    Tmap_attributes map_attr;
    Tlabel  label;

    ERRORDTree load_file(Ttacticaldatas &atrain_tacticaldatas,Tattributes &aattributes,
                         Tattributes &atrainattributes,const std::string afilename);
    ERRORDTree maketrainattr(Tattributes aattributes,Tattributes &atrainattributes);
    void match_properties(Ttacticaldatas atrain_tacticaldatas,
                          Tmap_attributes &amap_attr,Tlabel &label);
    void initpTree(TreeRoot &apTree){apTree = new Node(); }
    TreeRoot TreeGenerate(TreeRoot apTree,Ttacticaldatas &datas, Tattributes atrain_attributes,
                          Tmap_attributes amap_attr,Tlabel alabel);
    std::string majority_of_category(Ttacticaldatas datas,Tlabel alabel);
    bool belongs_same_label(Ttacticaldatas datas,std::string &alabel);
    Tmap_attributes optimal_attribute(Ttacticaldatas datas, Tattributes aattributes,
                                      Tmap_attributes amap_attr,Tlabel alabel);
    double calculate_information_entropy(Ttacticaldatas datas,Tmap_attributes amap_attr);//计算信息熵
    double calculate_splitinfo_gauge(Ttacticaldatas datas,std::map<std::string,
                                     std::vector<std::string>> aattr_map);//计算信息分裂度
    void  get_attr_vect( Ttacticaldatas datas,std::vector<std::string> &data_attr,std::string attr );
    double  calculate_condition_entropy(Ttacticaldatas datas,std::map<std::string,
                                        std::vector<std::string>> aattr_map,Tlabel alabel);//计算条件熵
    Ttacticaldatas remain_datas(Ttacticaldatas datas,std::string mapAttr,std::string attribute);
    Tattributes remain_attributes(Tattributes aattributes,std::string noneed_attr);

    //for predict
    std::string  PredictedRecursion(TreeRoot node, const Ttacticaldata data);//递归
    std::string  PredictedLoop(TreeRoot node, const Ttacticaldata data);//循环
    std::string  titlepath;
    std::string  treepath;

 //serialize
    std::vector<std::string> serialtree;
    void serialize(TreeRoot node,std::vector<std::string> &seritree);
    ERRORDTree ExportTree(const std::string afilename, std::vector<std::string> datastream);
 //deserialize
    std::vector<std::string> reserialtree;
    std::vector<std::string> g_reserialtree;//作为全局变量使用
    ERRORDTree Load_tree(TreeRoot &pTree,const std::string afilename);
    ERRORDTree ImportTree( std::string afilename,std::vector<std::string> &datastream);
    TreeRoot deserialize(TreeRoot node);
    TreeRoot deserialize(std::vector<std::string>serialtree,std::string label);
};




}//DecisionTree
}//Tactical
