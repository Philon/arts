package main

import (
	"io"
	"io/ioutil"
	"log"
	"os"
)

// 定制一套属于自己的logger
var (
	Trace   *log.Logger // 普通跟踪调试信息
	Info    *log.Logger // 特殊信息
	Warning *log.Logger // 警告日志
	Error   *log.Logger // 错误日志(输出到文件)
)

func init() {
	file, err := os.OpenFile("error.log", os.O_CREATE|os.O_WRONLY|os.O_APPEND, 0666)
	if err != nil {
		log.Fatalln("Failed to open error file: ", err)
	}

	// 因为ioutil.Discard，所有通过Trace打印的日志都不会输出
	Trace = log.New(ioutil.Discard, "TRACE: ", log.Ldate|log.Ltime|log.Lshortfile)
	Info = log.New(os.Stdout, "INFO: ", log.Ldate|log.Ltime|log.Lshortfile)
	Warning = log.New(os.Stdout, "WARNING: ", log.Ldate|log.Ltime|log.Lshortfile)
	// io.MultiWriter表示多种输出渠道，即同时打印到屏幕和文件
	Error = log.New(io.MultiWriter(file, os.Stderr), "ERROR: ", log.Ldate|log.Ltime|log.Llongfile)
}

func testMyLogger() {
	Trace.Println("I have something standard to say")
	Info.Println("Special Infomation")
	Warning.Println("There is something you need to know what")
	Error.Println("Something has failed")
}
