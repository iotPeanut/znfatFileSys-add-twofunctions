# 修改znfat文件系统，增加两个功能<br/>
---
`znfat文件系统` `查询文件数量` `修改文件名`<br/>
## (1) 写这两个函数的缘由
1、你会发现如果你文件名字是顺序的，比如1.txt 2.txt 3.txt....在单片机重新上电的时候，如何更快的找到需要新建第几个文件，
这时候只需要查询该文件目录下有几个文件就可以新建了<br/>
2、在物联网应用中，我们要确定那个文件中有没有发送到服务器的数据，就可以先把含有没有发送服务器数据的文件名字设置为含有通配符的，然后把该文件的数据发送完毕后，把文件名
改为正常的即可，这样很方便我觉得,不用在一遍一遍的去查找到底那个文件要发，或者断电的时候保存了，呵呵。<br/>
## (2) 说明
1、新增加的两个函数使用方法和原文件系统一样<br/>
2、znFAT_Test是stc15单片机写的一个测试两个函数功能的文件，晶振：22.1184 波特率：9600，返回信息会打印到串口上
