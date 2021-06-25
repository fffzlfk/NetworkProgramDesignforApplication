package main

import (
	"bufio"
	"fmt"
	"net"
	"strconv"
	"strings"

	"github.com/go-vgo/robotgo"
)

func process(conn net.Conn) {
	defer conn.Close()

	for {
		reader := bufio.NewReader(conn)
		var buf [1024]byte
		n, err := reader.Read(buf[:])
		if err != nil {
			continue
		}
		tmpStr := string(buf[:n])
		tmpStr = strings.TrimSpace(tmpStr)
		tmpStr = strings.Trim(tmpStr, "\n")
		strs := strings.Split(tmpStr, " ")
		for i := range strs {
			fmt.Println(strs[i])
			handleMouse(strs[i])
		}
	}
}

func handleMouse(str string) {
	strs := strings.Split(str, ",")
	len := len(strs)
	if len == 1 {
		if strings.Contains(str, "LEFT") {
			robotgo.MouseClick("left")
		} else if strings.Contains(str, "RIGHT") {
			robotgo.MouseClick("right")
		} else if strings.Contains(str, "SCROLLUP") {
			robotgo.ScrollMouse(5, "up")
		} else if strings.Contains(str, "SCROOLDOWN") {
			robotgo.ScrollMouse(5, "down")
		}
	} else if len == 3 {
		x, y := robotgo.GetMousePos()
		dx, _ := strconv.ParseFloat(strs[1], 64)
		dy, _ := strconv.ParseFloat(strs[2], 64)
		robotgo.MoveMouseSmooth(x+int(10*dx), y+int(10*dy), 1.0, 1.0)
	}
}

func main() {
	listen, err := net.Listen("tcp", "192.168.1.104:9090")
	if err != nil {
		panic(err)
	}
	for {
		conn, err := listen.Accept()
		if err != nil {
			continue
		}
		go process(conn)
	}
}
