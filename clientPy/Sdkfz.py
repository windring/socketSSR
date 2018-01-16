#-*- coding:utf-8 -*-

from tkinter import *
import socket
import _thread
import time

maki = Tk()
in1 = Entry(maki)
list = Listbox(maki)
s = socket.socket()

def sendMsg():
    msg=in1.get()+'\0'
    in1.delete(0,END)
    print(msg.encode("utf-8"))
    msg=msg.encode("gbk")
    s.sendall(msg)

def recvMsg():
    while (1):
        msg = s.recv(512)
        list.insert(0, msg.decode("gbk"))

def windowSSR():
    s.connect(("127.0.0.1", 141))
    maki.title("maki")
    in1.grid(row=0)
    in2 = Button(maki, text="send msg", command=sendMsg)
    in2.grid(row="1")
    list.grid(row=2)
    _thread.start_new_thread(recvMsg, ())
    maki.mainloop()
    # tkinter.messagebox.showinfo(message=msg)

windowSSR()
