
#include "tacticaldtree.h"

namespace Tactical {

namespace DecisionTree {

//根据属性找到每个样本对应的属性值
//std::vector<std::string> g_reserialtree;
ERRORDTree CTacticalDTree::findattr_value(Ttacticaldata data, std::string attr,std::string &attr_value)
{
    for(int i = 0 ; i<data.first.size();i++)
    {
        if(!data.first[i].compare(attr))
        {
            attr_value = data.second[i];
            return ERRORDTree::NOT_ERROR;
        }
    }
    return ERRORDTree::ERROR_FINDVALUE;
}



bool CTacticalDTree::belongs_same_label(Ttacticaldatas datas,std::string &alabel)
{
    std::string label = datas.begin()->second.back();

    for (auto iter = datas.begin(); iter != datas.end(); ++iter)
    {
        if (iter->second.back() != label)
        {
            return false;
        }
    }
    alabel = label;
    return true;
}

 void CTacticalDTree:: get_attr_vect( Ttacticaldatas datas,std::vector<std::string> &data_attr,std::string attr )
 {
     for(auto data : datas)
     {
         std::string adata_attr;
         findattr_value(data,  attr, adata_attr);
         data_attr.push_back(adata_attr);
     }
 }

 std::string CTacticalDTree :: majority_of_category(Ttacticaldatas datas,Tlabel alabel)
 {
     std::vector<std::string> data_attr;
     std::string majority_category;
     int count = 0;
     int countmax = 0;

     get_attr_vect(datas,data_attr,alabel.begin()->first);
     for(auto attrvalue: alabel.begin()->second)
     {
          count = std::count(data_attr.begin(),data_attr.end(),attrvalue);
         if(count > countmax)
         {
             countmax = count;
             majority_category = attrvalue;
         }
     }

     return majority_category;
 }


//计算信息熵
double CTacticalDTree :: calculate_information_entropy(Ttacticaldatas datas,std::map<std::string, std::vector<std::string>> aattr_map)
{

   std::vector<std::string> data_attr;
   std::vector<double> ratioattr;

   double entropy = 0.0;

   get_attr_vect( datas,data_attr, aattr_map.begin()->first);//获得数据中某个属性对应的属性值
   int size = data_attr.size();

   for(auto label:aattr_map.begin()->second)
   {
       int attrcount = 0;
       for(int i =0;i<data_attr.size();i++)
       {
           if(!data_attr[i].compare(label))
           {
               attrcount++;
           }
       }
       ratioattr.push_back((double)attrcount/size);
   }

   for(auto aratioattr: ratioattr)
   {
      entropy -= CalEntropy(aratioattr);
   }

   return entropy;

}

//计算条件熵
double CTacticalDTree :: calculate_condition_entropy(Ttacticaldatas datas,std::map<std::string, std::vector<std::string>> aattr_map,Tlabel alabel)
{
    Ttacticaldatas tmpdatas = datas;
    std::vector<std::string> data_attr;
    std::vector<std::string> label;
    std::vector<double> ratioattr;
    std::vector<double> posterior_entropy;
    std::string attr_value;
    double entropy = 0.0;

    get_attr_vect( tmpdatas,data_attr, aattr_map.begin()->first);//获得数据中某个属性对应的属性值
    int size = data_attr.size();
    for(auto label:aattr_map.begin()->second)
    {
        int attrcount = 0;
        for(int i =0;i<data_attr.size();i++)
        {
            if(!data_attr[i].compare(label))
            {
                attrcount++;
            }
        }
        ratioattr.push_back((double)attrcount/size);
    }

    for(auto label:aattr_map.begin()->second)//训练集中没有属性值所对应的数据集,那么就认为这个属性值的后验熵为0
    {
        Ttacticaldatas newdatas = remain_datas(tmpdatas,label,aattr_map.begin()->first);
        if(!newdatas.empty())
        {
            posterior_entropy.push_back(calculate_information_entropy(newdatas,alabel)); //calculate posterior_entropy
        }
        else
        {
           posterior_entropy.push_back(0);
        }
    }
   // calculate calculate condition entropy

    for(int i = 0;i < aattr_map.begin()->second.size();i++)
    {
        entropy += ratioattr[i]*posterior_entropy[i];
    }
    return entropy;
}
//计算分裂度量
double CTacticalDTree :: calculate_splitinfo_gauge(Ttacticaldatas datas,std::map<std::string, std::vector<std::string>> aattr_map)//计算信息分裂度
{
    double gauge = 0.0;
    Ttacticaldatas tmpdatas = datas;
    std::vector<std::string> data_attr;

    get_attr_vect( tmpdatas,data_attr, aattr_map.begin()->first);//获得数据中某个属性对应的属性值
    int size = data_attr.size();
    for(auto label:aattr_map.begin()->second)
    {
        gauge -= CalEntropy<double>((double)std::count(data_attr.begin(),data_attr.end(),label)/size);
    }
#ifdef INCLUDE_QT
    if(isnan(gauge)){qDebug() << "gauge is nan!";}
#endif
    return gauge;
}


//找出最优属性
Tmap_attributes CTacticalDTree :: optimal_attribute(Ttacticaldatas datas, Tattributes aattributes, Tmap_attributes amap_attr,Tlabel alabel)
{
    Tmap_attributes aoptimal_attributes;
    std::map<std::string, double> map_gains;
#ifdef METHOD_C45
    std::map<std::string, double> map_gains_ratio;
#endif
    double info_entropy = calculate_information_entropy(datas,alabel); //计算信息熵

    for (auto aattribute : aattributes)
    {
       Tmap_attributes atmp;
       atmp[aattribute]= amap_attr[aattribute];
       double map_gain = info_entropy - calculate_condition_entropy(datas,atmp,alabel);//信息增益 = 信息熵 - 条件熵
       map_gains[aattribute] = std::isnan(map_gain)? 0:map_gain;//
#ifdef METHOD_C45
       double split_gauge = calculate_splitinfo_gauge(datas,atmp);//分裂信息度
       map_gains_ratio[aattribute] = double(map_gain/split_gauge);// 信息增益率
#endif
    }

    auto compare_x_y = [](const std::pair<std::string, double> x, const std::pair<std::string, double> y)
    {
        return x.second > y.second;
    };
    std::vector<std::pair<std::string, double>> vec_map_gains(map_gains.begin(), map_gains.end());
    std::sort(vec_map_gains.begin(), vec_map_gains.end(), compare_x_y);
#ifdef METHOD_C45
    std::vector<std::pair<std::string, double>> vec_map_gains_ratio(map_gains_ratio.begin(),map_gains_ratio.end());
    std::sort(vec_map_gains_ratio.begin(), vec_map_gains_ratio.end(), compare_x_y);
    int gains_aboveavg_size = aattributes.size()/2;
    //c4.5选择标准:先从候选划分属性中找出信息增益高于平均水平的属性,再从中选择信息增益率最高的
    std::vector<std::string> vec_name_gains_aboveave ;
    for(int i = 0; i<gains_aboveavg_size ;i++)
    {
        vec_name_gains_aboveave.push_back(vec_map_gains[i].first);
    }
    for(auto gains_ratio: vec_map_gains_ratio)
    {
        if(std::find(vec_name_gains_aboveave.begin(),vec_name_gains_aboveave.end(),gains_ratio.first )!= vec_name_gains_aboveave.end())
        {
            auto search = amap_attr.find(gains_ratio.first);
            aoptimal_attributes[search->first] = search->second;
            return aoptimal_attributes;
        }
    }
#endif

    auto search = amap_attr.find(vec_map_gains[0].first);
    aoptimal_attributes[search->first] = search->second;
    return aoptimal_attributes;
}

Ttacticaldatas CTacticalDTree ::remain_datas(Ttacticaldatas datas,std::string mapAttr,std::string attribute) //根据属性和属性值筛选出新的训练集
{
    Ttacticaldatas anewdatas;
    std::string attr_value;
    for(auto data :datas )
    {
        findattr_value(data, attribute,attr_value);
        if(!attr_value.compare(mapAttr))
        {
            anewdatas.push_back(data);
        }
    }
    return anewdatas;
}

Tattributes CTacticalDTree :: remain_attributes(Tattributes aattributes,std::string noneed_attr)
{
    Tattributes new_attrs;
    for (auto aattribute : aattributes)
    {
        if (aattribute.compare(noneed_attr))//1 equal  0 not equal
        {
            new_attrs.push_back(aattribute);
        }
    }
    return new_attrs;
}

ERRORDTree CTacticalDTree::maketrainattr(Tattributes aattributes,Tattributes &atrainattributes)
{
   int j = 0;
   if((TRAINATTR_ED+1) > aattributes.size())
   {
       return ERRORDTree::ERROR_PARA;
   }
   for(int i = TRAINATTR_STR;i<TRAINATTR_ED+1;i++)//第一个数值是0
   {
        atrainattributes.push_back(aattributes[i]);
        j++;
   }
    return ERRORDTree::NOT_ERROR;
}
//CSV导出序列化后的决策树
ERRORDTree CTacticalDTree::ExportTree(const std::string afilename, std::vector<std::string> datastream)
{
    std::ofstream ostrm(afilename);
    if(!ostrm.is_open())
    {
        return ERRORDTree::ERROR_FWRITE;
    }
    else
    {
        for(int i = 0; i< datastream.size() - 1 ;i++)
        {
            ostrm<<datastream[i];
            ostrm<<",";
        }
        ostrm<<datastream[datastream.size()-1];
    }
    ostrm.close();
    return ERRORDTree::NOT_ERROR;
}

ERRORDTree CTacticalDTree:: ImportTree(const std::string afilename,std::vector<std::string> &datastream)
{
    std::ifstream istrm(afilename);
    if(!istrm.is_open())
    {
        return ERRORDTree::ERROR_FREAD;
    }
    else
    {
        std::vector<std::string> words = dataprocess->stream2vec(istrm);
        datastream.assign(words.begin(),words.end());
    }
    istrm.close();
    return ERRORDTree::NOT_ERROR;
}


//根据宏定义确定需要训练的属性位置
ERRORDTree CTacticalDTree ::load_file(Ttacticaldatas &atrain_tacticaldatas,Tattributes &aattributes,Tattributes &atrainattributes,const std::string afilename)
{

    std::ifstream istrm(afilename);
    if(!istrm.is_open())
    {
#ifdef INCLUDE_QT
        qDebug()<<"fread err!";
#endif
        return ERRORDTree::ERROR_FREAD;
    }
    else
    {
          char readbuffer[READBUFFERSIZE];
          bool firstline = true;

          char *token = NULL;
          while (istrm.peek()!= EOF)//0 not eof bit //1 eof bit
          {
              InitBuffer(readbuffer,READBUFFERSIZE);
              istrm.getline(readbuffer, READBUFFERSIZE);

              if(readbuffer[READBUFFERSIZE-1] != '\0')
              {
#ifdef INCLUDE_QT
                  qDebug()<<"readbuffer overflow!";
#endif
              }

              if (firstline)
              {
#ifndef CSV
                  char * attribute = STRTOK(readbuffer, " ",token);
#else
                  char * attribute = STRTOK(readbuffer, ",",token); //交叉编译器不能使用strtok_s
#endif
                  aattributes.push_back((std::string)(attribute));
                  while((*token) != '\0')
                  {
#ifndef CSV
                      attribute = STRTOK(token, " ",token);
#else
                      attribute = STRTOK(token, ",",token);
#endif
                      aattributes.push_back((std::string)(attribute));
                  }
                   maketrainattr(aattributes,atrainattributes);
                  firstline = false;
              }
              else
              {
                   Ttacticaldata atacticaldata;
                   atacticaldata.first.assign(aattributes.begin(),aattributes.end());
#ifndef CSV
                   char * data = STRTOK(readbuffer, " ",token);
#else
                   char * data = STRTOK(readbuffer, ",",token);
#endif
                   atacticaldata.second.push_back((std::string)(data));
                   while((*token) != '\0')
                   {
#ifndef CSV
                       data = STRTOK(token, " ",token);
#else
                       data = STRTOK(token, ",",token);
#endif
                       atacticaldata.second.push_back((std::string)(data));
                   }
                    atrain_tacticaldatas.push_back(atacticaldata);
                 //   qDebug()<<atrain_tacticaldatas.size();
              }
           }

    }
    istrm.close();
    return ERRORDTree::NOT_ERROR;
}

void CTacticalDTree::match_properties(Ttacticaldatas atrain_tacticaldatas,Tmap_attributes &amap_attr,Tlabel &alabel)
{
   Tattributes attrbutes = atrain_tacticaldatas[0].first;
   for(int i = 0; i<attrbutes.size();i++)
   {
       std::vector<std::string> attrTmp;
          for(auto data:atrain_tacticaldatas)
          {
              std::string attr_value;
              if(findattr_value(data,attrbutes[i],attr_value) == ERRORDTree::NOT_ERROR)
              {
                  if (!attrTmp.empty() &&
                          std::find(attrTmp.begin(), attrTmp.end(), attr_value) == attrTmp.end()) {
                      attrTmp.push_back(attr_value);
                  }
                  else if (attrTmp.empty()) {

                      attrTmp.push_back(attr_value);
                  }
              }
              else
              {
                  //missing data
              }

          }
          if(!attrbutes[i].compare(attrbutes.back()))//get label set //当label不是最后一个属性时,需要修改
          {
              alabel[attrbutes[i]] = attrTmp;
          }
       amap_attr[attrbutes[i]] = attrTmp;
   }


}

TreeRoot CTacticalDTree::TreeGenerate(TreeRoot pTree,Ttacticaldatas &datas, Tattributes atrain_attributes,Tmap_attributes amap_attr ,Tlabel alabel )
{
    std::string Dlabel;
    if(belongs_same_label(datas,Dlabel))
    {
        pTree->attribute = Dlabel;
        return pTree;
    }
    else
    {
        if(atrain_attributes.empty())
        {
                pTree->attribute = majority_of_category(datas,alabel);//数据训练集为空时
                return pTree;
#ifdef INCLUDE_QT
                qDebug()<<"atrain_attributes.empty!";
#endif

        }
        else
        {
            Tmap_attributes optimal_attrs = optimal_attribute(datas, atrain_attributes, amap_attr,alabel);//根据信息增益(ID3)计算出最优属性
            pTree->attribute = optimal_attrs.begin()->first;
            for (auto aptimal_attr : optimal_attrs.begin()->second)
            {
                 Node* new_node = new Node();
                 new_node->edgeValue = aptimal_attr;
                 Ttacticaldatas new_datas = remain_datas(datas, aptimal_attr, optimal_attrs.begin()->first);
                 if(new_datas.empty())
                 {
                    new_node->attribute = majority_of_category(datas,alabel);//数据训练集为空时
#ifdef INCLUDE_QT
                     qDebug()<<"new_datas.empty!";
#endif
                 }
                 else
                 {
                    Tattributes  anew_train_attributes = remain_attributes(atrain_attributes,optimal_attrs.begin()->first);
                    TreeGenerate(new_node, new_datas, anew_train_attributes, amap_attr,alabel);

                 }
                 pTree->childs.push_back(new_node);
            }
        }
    }
    return pTree;
}

ERRORDTree CTacticalDTree::Train_Dtree(TreeRoot &pTree,const std::string afilename)
{
    load_file(train_tacticaldatas,attributes,trainattributes,afilename);

    match_properties(train_tacticaldatas,map_attr,label);

    initpTree(pTree);

    pTree = TreeGenerate(pTree,train_tacticaldatas, trainattributes,map_attr,label);
     if(!pTree->attribute.compare(" ")){return ERRORDTree::ERROR_TRAIN;}

    serialize(pTree,serialtree);//serialize
    ExportTree(EXPORTTREE_ADDR, serialtree);

    return ERRORDTree::NOT_ERROR;
}

ERRORDTree CTacticalDTree::LOAD()
{
    if(!offl_pTree)
    {
       // std::string exporttree_addr = std::string(STRVALUE(TREE_PATH)) + "/TacticalplannerTree.csv";
        std::string exporttree_addr = treepath;
        TDEBUG("tree addr: %s", exporttree_addr.c_str());
        ERRORDTree ret;
        TDEBUG("load tree begin");
        ret = Load_tree(offl_pTree,exporttree_addr);
        if(ret != ERRORDTree::NOT_ERROR)
        {
          TDEBUG("load tree error");
        }
        TDEBUG("load tree end");
    }
    return ERRORDTree::NOT_ERROR;
}

ERRORDTree CTacticalDTree::Load_tree(TreeRoot &pTree,const std::string afilename) //加载离线的决策树
{
    ERRORDTree ERROR = ImportTree(afilename,reserialtree);
    if(ERROR != ERRORDTree::NOT_ERROR){return ERROR;}

    initpTree(pTree);

    pTree = deserialize(reserialtree,"non_recursion");
  //  g_reserialtree.assign(reserialtree.begin(),reserialtree.end());
 //   pTree = deserialize(pTree);
    return ERRORDTree::NOT_ERROR;
}

std::string CTacticalDTree:: PredictedRecursion(TreeRoot node, const Ttacticaldata data)
{
    TreeRoot newnode;
    if(node->childs.empty()) //0 not empty 1 empty
    {
        return node->attribute;
    }
    else
    {
        std::string attrvalue;
        findattr_value(data,node->attribute,attrvalue);
        for(size_t i = 0; i<node->childs.size();i++)
        {
            if(!node->childs[i]->edgeValue.compare(attrvalue))
            {
                  newnode =  node->childs[i];
                  break;
            }
        }

        return PredictedRecursion(newnode,data);
    }
}


std::string CTacticalDTree:: PredictedLoop(TreeRoot node, const Ttacticaldata data)
{

        while(!node->childs.empty())
        {
            std::string attrvalue;
            findattr_value(data,node->attribute,attrvalue);
            for(size_t i = 0; i<node->childs.size();i++)
            {
                if(!node->childs[i]->edgeValue.compare(attrvalue))
                {
                      node =  node->childs[i];
                      break;
                }
                if(i == (node->childs.size() -1))
                {
                    return " ";
                }
            }
        }
    return node->attribute;
}

std::string CTacticalDTree::RESULT(std::vector<std::string> predictdata)
{
    std::string result;
    Ttacticaldata predict_tacticaldata;
    if(predict_attribute.empty())
    {   //std::string titlepath = titlepath;
        TDEBUG("load title: %s",titlepath.c_str());
        ImportTitle(titlepath, predict_attribute);
    }
    predict_tacticaldata.first.assign(predict_attribute.begin(),(predict_attribute.end()-1));
    predict_tacticaldata.second.assign(predictdata.begin(),predictdata.end());
    if(predict_tacticaldata.first.size() != predict_tacticaldata.second.size())
    {
        TDEBUG("data error");
        return "data error";
    }
    return Predict(offl_pTree,predict_tacticaldata);
}
std::string CTacticalDTree::Predict(TreeRoot node,const Ttacticaldata apredict_tacticaldata)
{
   // return PredictedRecursion(node,apredict_tacticaldata);
      return PredictedLoop(node,apredict_tacticaldata);
}
//序列化采用N叉树前序遍历的方式,不含子节点的节点用#标记,含有子节点的节点会记录子节点的个数
//| attribute | edgevalue | size of child/# | chaild 1 | child 2|.............
void CTacticalDTree::serialize(TreeRoot node,std::vector<std::string> &seritree)
{
    if(node->childs.empty())
    {
#ifdef INCLUDE_QT
#if(0)
        qDebug()<<QString::fromStdString(node->attribute)<<"<-";
        qDebug()<<QString::fromStdString(node->edgeValue);
        qDebug()<<"#";
#endif
#endif
        seritree.push_back(node->attribute);
        seritree.push_back(node->edgeValue);
        seritree.push_back("#");
        return ;
    }
#ifdef INCLUDE_QT
#if(0)
    qDebug()<<QString::fromStdString(node->attribute)<<"<-";
    qDebug()<<QString::fromStdString(node->edgeValue);
    qDebug()<<node->childs.size();
#endif
#endif
    int size = node->childs.size();
    seritree.push_back(node->attribute);
    seritree.push_back(node->edgeValue);
    seritree.push_back(std::to_string(size));
    for(size_t i = 0; i< node->childs.size();i++)
    {
        serialize(node->childs[i],seritree);
    }
    return;
}


TreeRoot CTacticalDTree::deserialize(TreeRoot node)
{
    node->attribute = g_reserialtree.at(0);
    node->edgeValue = g_reserialtree.at(1);

    if(!g_reserialtree.at(2).compare("#"))
    {
        g_reserialtree.erase(g_reserialtree.begin(),g_reserialtree.begin()+3);
        return node;
    }
    else
    {
        int childsize = std::atoi(g_reserialtree.at(2).c_str());
        g_reserialtree.erase(g_reserialtree.begin(),g_reserialtree.begin()+3);
        for(int i =0; i<childsize;i++)
        {
            TreeRoot newnode = new Node();
            node->childs.push_back(deserialize(newnode));

        }
        return node;
    }

}

TreeRoot CTacticalDTree::deserialize(std::vector<std::string>serialtree,std::string label)
{
     std::stack<TreeRoot> stack;
     std::stack<int>      childsize;
     TreeRoot head =  new Node();
     head->attribute = serialtree[0];
     head->edgeValue = serialtree[1];
     childsize.push(std::atoi(serialtree[2].c_str()));
     stack.push(head);

     serialtree.erase(serialtree.begin(),serialtree.begin()+3);
     do
     {
         TreeRoot tmp = new Node();
         tmp->attribute = serialtree[0];
         tmp->edgeValue = serialtree[1];
         std::string flag = serialtree[2];
         serialtree.erase(serialtree.begin(),serialtree.begin()+3);

         if(flag.compare("#"))//不是叶子节点
         {
            stack.top()->childs.push_back(tmp);
            childsize.top() = childsize.top()--;
            stack.push(tmp);
            childsize.push(std::atoi(flag.c_str()));
         }
         else
         {
            stack.top()->childs.push_back(tmp);
            childsize.top() = childsize.top()--;
            while(childsize.top() == 0)
            {

                stack.pop();
                childsize.pop();
                if(stack.empty() || childsize.empty())
                {
                    break;
                }

            }
         }

     }while(!serialtree.empty());

     return head;
}


}//DecisionTree
}//Tactical
