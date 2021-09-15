#!/usr/bin/env python
#coding=UTF-8
import rospy #ros
from std_msgs.msg import String # ros 字串訊息
import keyboard # 按鍵


# 訂閱 訊息處理器 （接收到訊息）
def callback(data): 
  print("ss:" + data.data)

def move():
  key = []
  rospy.init_node('espText', anonymous=True) # 初始化 節點
  pub = rospy.Publisher('/message', String, queue_size=10) # 初始化 發布
  rospy.Subscriber("chatter", String, callback) # 訂閱 （被訂閱名稱，訊息類型，訊息處理函式）

  while not rospy.is_shutdown():
      Str=str(keyboard.read_key()) # 接聽到得按鍵 =》 Str
      key.append(Str)
      Max = len(key)
      print(Max)
      if (len(key)==2): #read_key會讀取一按一放所以需要list存取兩次按鍵並且只讀第一次按鍵回傳Arduino
        key = []
      else:
        rospy.loginfo(key[0]) # 提示訊息
        pub.publish(key[0]) # 訊息發布
      rospy.Rate(100) # 等待訊息發完
if __name__ == '__main__':
  try:
      move()
  except rospy.ROSInterruptException: pass