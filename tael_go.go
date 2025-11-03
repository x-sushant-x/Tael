package main

import (
	"bufio"
	"fmt"
	"io"
	"os"
	"strings"
	"time"
)

const (
	defaultLines   = 10
	pollIntervalMs = 100
)

func printError(msg string) {
	fmt.Printf("\033[31mERROR:\033[0m %s\n", msg)
}

func printErrorWithInstructions(msg, inst string) {
	printError(msg)
	fmt.Printf("\n%s\n", inst)
}

func readLastLines(file *os.File, linesToRead int) (int64, error) {
	stat, err := file.Stat()
	if err != nil {
		return -1, err
	}
	size := stat.Size()
	if size == 0 {
		return 0, nil
	}

	var pos = size - 1
	var newLines int
	buf := make([]byte, 1)

	for pos >= 0 {
		_, err := file.Seek(pos, io.SeekStart)
		if err != nil {
			return -1, err
		}
		_, err = file.Read(buf)
		if err != nil {
			return -1, err
		}
		if buf[0] == '\n' {
			newLines++
			if newLines == linesToRead {
				break
			}
		}
		pos--
	}

	startPos := int64(0)
	if newLines == linesToRead {
		startPos = pos + 1
	}

	_, err = file.Seek(startPos, io.SeekStart)
	if err != nil {
		return -1, err
	}

	reader := bufio.NewReader(file)
	for {
		line, err := reader.ReadString('\n')
		fmt.Print(line)
		if err == io.EOF {
			break
		} else if err != nil {
			return -1, err
		}
	}

	return size, nil
}

func followFile(fileName string, file *os.File, lastReadPos int64) error {
	for {
		time.Sleep(pollIntervalMs * time.Millisecond)

		stat, err := os.Stat(fileName)
		if err != nil {
			continue
		}
		newSize := stat.Size()

		// File truncated or rotated
		if newSize < lastReadPos {
			file.Close()
			file, err = os.Open(fileName)
			if err != nil {
				return err
			}
			lastReadPos = 0
		}

		if newSize > lastReadPos {
			_, err := file.Seek(lastReadPos, io.SeekStart)
			if err != nil {
				return err
			}
			reader := bufio.NewReader(file)
			for {
				line, err := reader.ReadString('\n')
				if len(line) > 0 {
					fmt.Print(line)
				}
				if err == io.EOF {
					break
				} else if err != nil {
					return err
				}
			}
			lastReadPos = newSize
		}
	}
}

func main() {
	if len(os.Args) < 3 {
		printErrorWithInstructions("Invalid Command", "Usage: tael <option> <file_name> or run tael -help for more information.")
		return
	}

	option := os.Args[1]
	fileName := os.Args[2]

	file, err := os.Open(fileName)
	if err != nil {
		printError("File not found.")
		return
	}
	defer file.Close()

	switch {
	case option == "-f":
		lastReadPos, err := readLastLines(file, defaultLines)
		if err != nil {
			printError("Unable to read file.")
			return
		}
		err = followFile(fileName, file, lastReadPos)
		if err != nil {
			printError(fmt.Sprintf("Error following file: %v", err))
		}

	case strings.HasPrefix(option, "-r"):
		linesToRead := defaultLines
		if len(option) > 2 {
			numStr := option[2:]
			var num int
			_, err := fmt.Sscanf(numStr, "%d", &num)
			if err != nil || num <= 0 {
				printErrorWithInstructions("Invalid usage of -r flag.", "Correct usage: tael -r<lines> file_name or just -r to read 10 lines.")
				return
			}
			linesToRead = num
		}

		_, err := readLastLines(file, linesToRead)
		if err != nil {
			printError("Unable to read file.")
			return
		}

	default:
		printErrorWithInstructions("Invalid Command", "Usage: tael <option> <file_name> or run tael -help for more information.")
	}
}
