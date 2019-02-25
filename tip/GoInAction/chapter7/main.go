package main

import (
	"log"
	"os"
	"time"
)

func runnerTest() {
	log.Println("Runner test starting work...")

	r := New(5 * time.Second)
	for i := 0; i < 5; i++ {
		r.Add(func(id int) {
			log.Printf("Processor - Task #%d.\n", id)
			time.Sleep(time.Duration(id) * time.Second)
			log.Printf("Task #%d done.\n", id)
		})
	}

	if err := r.Start(); err != nil {
		switch err {
		case ErrTimeout:
			log.Println("Terminating due to timeout.")
			os.Exit(1)
		case ErrInterrupt:
			log.Println("Terminating due to interrupt.")
			os.Exit(2)
		}
	}

	log.Println("Process end.")
}

func main() {
	runnerTest()
}
