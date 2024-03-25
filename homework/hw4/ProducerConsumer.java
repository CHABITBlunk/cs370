package hw4;

public class ProducerConsumer {

  public static void main(String[] args) {
    BoundedBuffer buffer = new BoundedBuffer(1000);
    Producer p = new Producer(buffer, 1000000);
    Consumer c = new Consumer(buffer, 1000000);
    p.start();
    c.start();
    try {
      p.join();
      c.join();
    } catch (InterruptedException e) {
      Thread.currentThread().interrupt();
    }
    System.out.println("Exiting!");
  }
}
