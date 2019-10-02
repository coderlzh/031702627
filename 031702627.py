#!/usr/bin/env python
# -*- coding: utf-8 -*- 

import profile
import json
import re

jsonstr='{"台湾省":["台北市","新北市","桃园市","台中市","台南市","高雄市","基隆市","新竹市","嘉义市"],"澳门特别行政区":["花地玛堂区","圣安多尼堂区","大堂区","望德堂区","风顺堂区","嘉模堂区","圣方济各堂区"],"香港特别行政区":["九龙城区","北区","中西区","东区","南区","湾仔区","观塘区","深水埗区","黄大仙区","油尖旺区","离岛区","葵青区","西贡区","西贡区","沙田区","大埔区","荃湾区","屯门区","元朗区"],"北京":["北京市"],"天津":["天津市"],"河北省":["石家庄市","唐山市","秦皇岛市","邯郸市","邢台市","保定市","张家口市","承德市","沧州市","廊坊市","衡水市"],"山西省":["太原市","大同市","阳泉市","长治市","晋城市","朔州市","晋中市","运城市","忻州市","临汾市","吕梁市"],"内蒙古自治区":["呼和浩特市","包头市","乌海市","赤峰市","通辽市","鄂尔多斯市","呼伦贝尔市","巴彦淖尔市","乌兰察布市","兴安盟","锡林郭勒盟","阿拉善盟"],"辽宁省":["沈阳市","大连市","鞍山市","抚顺市","本溪市","丹东市","锦州市","营口市","阜新市","辽阳市","盘锦市","铁岭市","朝阳市","葫芦岛市"],"吉林省":["长春市","吉林市","四平市","辽源市","通化市","白山市","松原市","白城市","延边朝鲜族自治州"],"黑龙江省":["哈尔滨市","齐齐哈尔市","鸡西市","鹤岗市","双鸭山市","大庆市","伊春市","佳木斯市","七台河市","牡丹江市","黑河市","绥化市","大兴安岭地区"],"上海":["上海市"],"江苏省":["南京市","无锡市","徐州市","常州市","苏州市","南通市","连云港市","淮安市","盐城市","扬州市","镇江市","泰州市","宿迁市"],"浙江省":["杭州市","宁波市","温州市","嘉兴市","湖州市","绍兴市","金华市","衢州市","舟山市","台州市","丽水市"],"安徽省":["合肥市","芜湖市","蚌埠市","淮南市","马鞍山市","淮北市","铜陵市","安庆市","黄山市","滁州市","阜阳市","宿州市","六安市","亳州市","池州市","宣城市"],"福建省":["福州市","厦门市","莆田市","三明市","泉州市","漳州市","南平市","龙岩市","宁德市"],"江西省":["南昌市","景德镇市","萍乡市","九江市","新余市","鹰潭市","赣州市","吉安市","宜春市","抚州市","上饶市"],"山东省":["济南市","青岛市","淄博市","枣庄市","东营市","烟台市","潍坊市","济宁市","泰安市","威海市","日照市","莱芜市","临沂市","德州市","聊城市","滨州市","菏泽市"],"河南省":["郑州市","开封市","洛阳市","平顶山市","安阳市","鹤壁市","新乡市","焦作市","濮阳市","许昌市","漯河市","三门峡市","南阳市","商丘市","信阳市","周口市","驻马店市","济源市"],"湖北省":["武汉市","黄石市","十堰市","宜昌市","襄阳市","鄂州市","荆门市","孝感市","荆州市","黄冈市","咸宁市","随州市","恩施土家族苗族自治州","仙桃市","潜江市","天门市","神农架林区"],"湖南省":["长沙市","株洲市","湘潭市","衡阳市","邵阳市","岳阳市","常德市","张家界市","益阳市","郴州市","永州市","怀化市","娄底市","湘西土家族苗族自治州"],"广东省":["广州市","韶关市","深圳市","珠海市","汕头市","佛山市","江门市","湛江市","茂名市","肇庆市","惠州市","梅州市","汕尾市","河源市","阳江市","清远市","东莞市","中山市","潮州市","揭阳市","云浮市"],"广西壮族自治区":["南宁市","柳州市","桂林市","梧州市","北海市","防城港市","钦州市","贵港市","玉林市","百色市","贺州市","河池市","来宾市","崇左市"],"海南省":["海口市","三亚市","三沙市","儋州市","五指山市","琼海市","文昌市","万宁市","东方市","定安县","屯昌县","澄迈县","临高县","白沙黎族自治县","昌江黎族自治县","乐东黎族自治县","陵水黎族自治县","保亭黎族苗族自治县","琼中黎族苗族自治县"],"重庆":["重庆市"],"四川省":["成都市","自贡市","攀枝花市","泸州市","德阳市","绵阳市","广元市","遂宁市","内江市","乐山市","南充市","眉山市","宜宾市","广安市","达州市","雅安市","巴中市","资阳市","阿坝藏族羌族自治州","甘孜藏族自治州","凉山彝族自治州"],"贵州省":["贵阳市","六盘水市","遵义市","安顺市","毕节市","铜仁市","黔西南布依族苗族自治州","黔东南苗族侗族自治州","黔南布依族苗族自治州"],"云南省":["昆明市","曲靖市","玉溪市","保山市","昭通市","丽江市","普洱市","临沧市","楚雄彝族自治州","红河哈尼族彝族自治州","文山壮族苗族自治州","西双版纳傣族自治州","大理白族自治州","德宏傣族景颇族自治州","怒江傈僳族自治州","迪庆藏族自治州"],"西藏自治区":["拉萨市","日喀则市","昌都市","林芝市","山南市","那曲市","阿里地区"],"陕西省":["西安市","铜川市","宝鸡市","咸阳市","渭南市","延安市","汉中市","榆林市","安康市","商洛市"],"甘肃省":["兰州市","嘉峪关市","金昌市","白银市","天水市","武威市","张掖市","平凉市","酒泉市","庆阳市","定西市","陇南市","临夏回族自治州","甘南藏族自治州"],"青海省":["西宁市","海东市","海北藏族自治州","黄南藏族自治州","海南藏族自治州","果洛藏族自治州","玉树藏族自治州","海西蒙古族藏族自治州"],"宁夏回族自治区":["银川市","石嘴山市","吴忠市","固原市","中卫市"],"新疆维吾尔自治区":["乌鲁木齐市","克拉玛依市","吐鲁番市","哈密市","昌吉回族自治州","博尔塔拉蒙古自治州","巴音郭楞蒙古自治州","阿克苏地区","克孜勒苏柯尔克孜自治州","喀什地区","和田地区","伊犁哈萨克自治州","塔城地区","阿勒泰地区","石河子市","阿拉尔市","图木舒克市","五家渠市","铁门关市"]}'


address=json.loads(jsonstr)



def get_province(string_input):         #省份提取函数，遍历地址表的key值进行匹配
    address_input = string_input
    for province in address.keys():
        matchobj = re.match(address_input[0:2],province)
        if(matchobj):
            return province

def get_city(string_input,province):    #城市提取函数，遍历省份对应的链表
    if(province == "北京" or province == "上海" or province == "重庆" or province == "天津"):       #直辖市特殊处理
        return province + "市"
    address_input = string_input
    for index in range(len(address[province])):
        matchobj = re.match(address_input[0:2],address[province][index])
        if(matchobj):
            return address[province][index]
        
def get_village(string_input):          #三级地址提取函数，正则匹配法，收集关键字进行匹配
    address_input = string_input
    searchobj = re.search("\S*?区",address_input)  
    if(searchobj):
        return searchobj.group()
    searchobj = re.search("\S*?县",address_input)
    if(searchobj):
        return searchobj.group()
    searchobj = re.search("\S*?市",address_input)
    if(searchobj):
        return searchobj.group()

def get_town(string_input):             #四级地址提取函数，正则匹配法，收集关键字进行匹配
    address_input = string_input
    searchobj = re.search("\S*?街道",address_input)  
    if(searchobj):
        return searchobj.group()
    searchobj = re.search("\S*?乡",address_input)
    if(searchobj):
        return searchobj.group()
    searchobj = re.search("\S*?镇",address_input)
    if(searchobj):
        return searchobj.group()

def get_road(string_input):             #五级地址提取函数
    address_input = string_input
    searchobj = re.search("\S*?路",address_input)  
    if(searchobj):
        return searchobj.group()
    searchobj = re.search("\S*?街",address_input)
    if(searchobj):
        return searchobj.group()
    searchobj = re.search("\S*?弄",address_input)
    if(searchobj):
        return searchobj.group()
def get_num(string_input):              #六级地址提取函数
    address_input = string_input
    searchobj = re.search("\S*?号",address_input)  
    if(searchobj):
        return searchobj.group()
    

def get_phone(string_input):
    phone_input = string_input
    searchobj = re.search("\d{11}",phone_input)
    #print("%s" %searchobj.group())
    return searchobj.group()

def get_name(string_input):
    name_input = string_input
    searchobj = re.search("\S*,",name_input)
    return searchobj.group(0)



def main(inputraw):
    dict={}
    string_input=inputraw
    try:
        type = string_input[0]  #记录类型
    except:
        print("错误，禁止输入空字符串")
        return -1
    string_input = re.sub("1!","",string_input,1)   #预处理
    string_input = re.sub("2!","",string_input,1)
    string_input = re.sub("3!","",string_input,1)
    dict["姓名"] = get_name(string_input)[:-1]    #需要先提取姓名，防止姓名中存在关键词：省、市之类的
    string_input =re.sub(get_name(string_input),"",string_input,1)     #删除已选出的姓名信息  
    string_input = re.sub("省","",string_input,1)    #预处理
    #string_input = re.sub("市","",string_input,1)
    string_input = re.sub("\.","",string_input,1)
    #print("%s\n" %string_input)
    #print("%s\n" %string_input)
    dict["手机"] = get_phone(string_input)    #提取手机号
    string_input =re.sub(get_phone(string_input),"",string_input,1) #删除已选出的手机信息
    #print("%s\n" %string_input)
    if(get_province(string_input)):
        dict.setdefault("地址",[]).append(get_province(string_input)) #提取地址所在省，并放入字典中的“地址”部分
        string_input =re.sub(get_province(string_input)[:-1],"",string_input,1) #删除已选出的省份信息
    else:
        dict.setdefault("地址",[]).append("")
    #print("%s\n" %string_input)
    if(string_input[0]=="区"):                               #打表处理直辖市问题
        string_input = re.sub("区","",string_input,1)
    if(string_input[0]=="京"):
        string_input = re.sub("京","",string_input,1)
    if(string_input[0]=="津"):
        string_input = re.sub("津","",string_input,1)
    if(string_input[0]=="海"):
        string_input = re.sub("海","",string_input,1)
    if(string_input[0]=="庆"):
        string_input = re.sub("庆","",string_input,1)
        #print("%s\n" %string_input)
    if(get_city(string_input,dict["地址"][0])):           #提取地址所在市，并判断是否有相关信息
        dict.setdefault("地址",[]).append(get_city(string_input,dict["地址"][0]))   #有，提取加入字典并删除已选出的城市信息
        string_input =re.sub(get_city(string_input,dict["地址"][0])[:-1],"",string_input,1)     
    else:
        dict.setdefault("地址",[]).append("") #没有，不执行删除操作，在字典中加入空字符
        #print("%s\n" %string_input)
    if(string_input[0]=="市"):                        #打表       
        string_input = re.sub("市","",string_input,1)
    if(string_input[0]=="州"):                               
        string_input = re.sub("州","",string_input,1)
    if(string_input[0]=="县"):                               
        string_input = re.sub("县","",string_input,1)
    if(string_input[0]=="盟"):                               
        string_input = re.sub("盟","",string_input,1)
    if(get_village(string_input)):                      #与提取城市同理
        dict.setdefault("地址",[]).append(get_village(string_input))
        string_input =re.sub(get_village(string_input),"",string_input,1)
    else:
        dict.setdefault("地址",[]).append("")
        #print("%s\n" %string_input)
    if(get_town(string_input)):                         #同理
        dict.setdefault("地址",[]).append(get_town(string_input))
        string_input =re.sub(get_town(string_input),"",string_input,1)
    else:
        dict.setdefault("地址",[]).append("")
    #print("%s\n" %string_input)
    if(type == "1"):                                #当类型为1时，只需要提供五级地址，将剩下的字符串添加进字典即可
        dict.setdefault("地址",[]).append(string_input)
    if(type == "2" or type == "3"):                 #当类型为2时，需要提供七级地址，调用get_road函数和get_num函数获取路名和门牌号    3功能因为受限于只有2级地址表难以实现，所以并在2功能中，实力不济，请谅解
        if(get_road(string_input)):                     #同理
            dict.setdefault("地址",[]).append(get_road(string_input))
            string_input =re.sub(get_road(string_input),"",string_input,1)
        else:
            dict.setdefault("地址",[]).append("")
        if(get_num(string_input)):                     #同理
            dict.setdefault("地址",[]).append(get_num(string_input))
            string_input =re.sub(get_num(string_input),"",string_input,1)
        else:
            dict.setdefault("地址",[]).append("")
        dict.setdefault("地址",[]).append(string_input)
    
    dict_json = json.dumps(dict) 
    print(dict_json)
    #print(dict)
while 1:
    try:
        inputraw = input();
        if(inputraw == "END"):
            break
    except EOFError:
        break
    main(inputraw)
#main()  #执行组函数
