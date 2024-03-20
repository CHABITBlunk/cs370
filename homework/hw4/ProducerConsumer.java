package hw4;

public class ProducerConsumer {

  public ProducerConsumer() {
    super();
  }

  public static void main(String[] args) {
    BoundedBuffer buffer = new BoundedBuffer();
    Consumer c = new Consumer(buffer);
    Producer p = new Producer(buffer);
  }
}
