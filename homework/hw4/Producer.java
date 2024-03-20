package hw4;


public class Producer implements Thread {

  private BoundedBuffer buffer;

  public Producer(BoundedBuffer b) { buffer = b; }
}
