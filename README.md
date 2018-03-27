# chatrom
https://blog.csdn.net/lishang6257/article/details/79711361


[toc]

##前言
$\qquad$这学期正好有门TCP/IP 编程，乘机写了个dos下的聊天室，去年这个时候用Java写了个带GUI的多人聊天室，不过整体来看这次在对网络编程有了一定的理论基础后，这次用c++写的时候，那叫一个轻车熟路。写了多个版本的demo,在文章的最后会把demo更新描述。（对于Java版或者Demo有需要的可以在评论区回复）。
##聊天室优点与可拓展功能
###优点：
1.实现了基本的聊天功能，包括群聊与私聊
2.用户注册时使用文件做过渡，用户退出登陆时仍可保留用户信息
3.封装消息传递时格式，包括消息的生成与解析
4.友好的操作逻辑
5.多方位错误提醒

###可拓展功能
1.本次由于要求，将客户端一分为二包括接收端和发送端，虽然减少了开多线程带来的工作量，但是使整个聊天室的一体性大打折扣，于是就可以在客户端开多线程去优化。
2.这聊天室限于其类型，只是通过文档来保存用户列表，但是可以使用数据库来增加用户量增大带来的问题
3.聊天室并不支持文件发送，可以考虑后期添加
 4.聊天室未对多群组，好友关系留下拓展接口，这是考虑到文件操作的局限性，如果有需求，可以做两方面考量，第一是使用数据库建立好友关系，群组关系，用户表；第二修改消息格式，拓展对添加的群组选择拓展。
##实测
###登陆界面

![这里写图片描述](https://img-blog.csdn.net/20180327122423278?watermark/2/text/aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2xpc2hhbmc2MjU3/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70)


###注册

![这里写图片描述](https://img-blog.csdn.net/20180327122658907?watermark/2/text/aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2xpc2hhbmc2MjU3/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70)

###聊天
![这里写图片描述](https://img-blog.csdn.net/20180327122939447?watermark/2/text/aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2xpc2hhbmc2MjU3/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70)

###群聊

![这里写图片描述](https://img-blog.csdn.net/20180327123430300?watermark/2/text/aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2xpc2hhbmc2MjU3/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70)

###私聊
![这里写图片描述](https://img-blog.csdn.net/20180327123449657?watermark/2/text/aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2xpc2hhbmc2MjU3/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70)

###退出（左边为sender退出，右边为receiver退出）
![这里写图片描述](https://img-blog.csdn.net/20180327123506762?watermark/2/text/aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2xpc2hhbmc2MjU3/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70)


##后记
$\qquad$现附上代码，有空在补上内部实现的亮点与不足，有什么问题，可以随时私信。有空可以follow下博主的[github](https://github.com/lishang6257)（丑不要碧莲，有打广告）.最后附上下载链接，就放在csdn,免费下载，喜欢点赞就行。







