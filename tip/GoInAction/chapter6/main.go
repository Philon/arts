package main

func main() {
	println("----------------CreateGoroutines----------------")
	CreateGoroutines()
	println("----------------AccessResource----------------")
	AccessResource()
	println("----------------AccessResourceByAtomic----------------")
	AccessResourceByAtomic()
	println("----------------AccessResourceByMutex----------------")
	AccessResourceByMutex()

	println()
	println("----------------UnbufferChannel----------------")
	UnbufferChannel()
	println("----------------BufferChannel----------------")
	BufferChannel()
}
