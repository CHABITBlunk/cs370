# list of files
JAVA_SRCS = BoundedBuffer.java Consumer.java Producer.java ProducerConsumer.java

# compiler, loader commands, & flags
JAVAC = javac
JAVAC_FLAGS = -encoding utf-8

all:
	$(JAVAC) $(JAVAC_FLAGS) $(JAVA_SRCS)

clean:
	rm -rf *.class *.zip

package:
	zip -r Habit-Blunk-HW4.zip $(JAVA_SRCS) Makefile README.txt
