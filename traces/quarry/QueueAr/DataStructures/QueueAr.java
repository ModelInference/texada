package DataStructures;

// QueueAr class
//

// CONSTRUCTION: with or without a capacity; default is 10
//
// ******************PUBLIC OPERATIONS*********************
// void enqueue( x )      --> Insert x
// Object getFront( )     --> Return least recently inserted item
// Object dequeue( )      --> Return and remove least recent item
// boolean isEmpty( )     --> Return true if empty; else false
// boolean isFull( )      --> Return true if capacity reached
// void makeEmpty( )      --> Remove all items
// ******************ERRORS********************************
// Overflow thrown for enqueue on full queue

/**
 * Array-based implementation of the queue.
 * @author Mark Allen Weiss
 */
public class QueueAr
{
  /**
   * Construct the queue.
   **/
  public QueueAr( )
  {
    this( DEFAULT_CAPACITY );
  }

  /**
   * Construct the queue.
   **/
  public QueueAr( int capacity )
  {
    theArray = new Object[ capacity ];
    makeEmpty( );
  }

  /**
   * Test if the queue is logically empty.
   * @return true if empty, false otherwise.
   **/
  public boolean isEmpty( )
  {
    return currentSize == 0;
  }

  /**
   * Test if the queue is logically full.
   * @return true if full, false otherwise.
   **/
  public boolean isFull( )
  {
    return currentSize == theArray.length;
  }

  /**
   * Make the queue logically empty.
   **/
  public void makeEmpty( )
  {
    currentSize = 0;
    front = 0;
    back = -1;
  }

  /**
   * Get the least recently inserted item in the queue.
   * Does not alter the queue.
   * @return the least recently inserted item in the queue, or null, if empty.
   **/
  public Object getFront( )
  {
    if( isEmpty( ) )
      return null;
    return theArray[ front ];
  }

  /**
   * Return and remove the least recently inserted item from the queue.
   * @return the least recently inserted item in the queue, or null, if empty.
   **/
  public Object dequeue( )
  {
    if( isEmpty( ) )
      return null;
    currentSize--;

    Object frontItem = theArray[ front ];
    theArray[ front ] = null;
    front = increment( front );
    return frontItem;
  }

  /**
   * Insert a new item into the queue.
   * @param x the item to insert.
   * @exception Overflow if queue is full.
   **/
  public void enqueue( Object x ) throws Overflow
  {
    if( isFull( ) )
      throw new Overflow( );
    back = increment( back );
    theArray[ back ] = x;
    currentSize++;
  }

  /**
   * Internal method to increment with wraparound.
   * @param x any index in theArray's range.
   * @return x+1, or 0, if x is at the end of theArray.
   **/
  private int increment( int x )
  {
    if( ++x == theArray.length )
      x = 0;
    return x;
  }

  private Object [ ] theArray;
  private int        currentSize;
  private int        front;
  private int        back;

  static final int DEFAULT_CAPACITY = 10;

  public static void main( String [ ] args )
  {
    QueueAr q = new QueueAr( );

    try
      {
        for( int i = 0; i < 10; i++ )
          q.enqueue( new MyInteger( i ) );

        while( !q.isEmpty( ) )
          System.out.println( q.dequeue( ) );

        int num = 68;
        q.enqueue(new MyInteger(num++));
        q.enqueue(new MyInteger(num++));
        q.enqueue(new MyInteger(num++));
        q.enqueue(new MyInteger(num++));
        q.enqueue(new MyInteger(num++));
        q.enqueue(new MyInteger(num++));
        // 6
        System.out.println( q.dequeue( ) );
        System.out.println( q.dequeue( ) );
        System.out.println( q.dequeue( ) );
        System.out.println( q.dequeue( ) );
        // 2
        q.enqueue(new MyInteger(num++));
        q.enqueue(new MyInteger(num++));
        q.enqueue(new MyInteger(num++));
        // 5
        System.out.println( q.dequeue( ) );
        System.out.println( q.dequeue( ) );
        // 3
        q.enqueue(new MyInteger(num++));
        q.enqueue(new MyInteger(num++));
        // 5
        System.out.println( q.dequeue( ) );
        System.out.println( q.dequeue( ) );
        System.out.println( q.dequeue( ) );
        // 2
        q.enqueue(new MyInteger(num++));
        q.enqueue(new MyInteger(num++));
        q.enqueue(new MyInteger(num++));
        q.enqueue(new MyInteger(num++));
        q.enqueue(new MyInteger(num++));
        q.enqueue(new MyInteger(num++));
        q.enqueue(new MyInteger(num++));
        q.enqueue(new MyInteger(num++));
        // 10
        System.out.println( q.dequeue( ) );
        System.out.println( q.dequeue( ) );
        System.out.println( q.dequeue( ) );
        System.out.println( q.dequeue( ) );
        System.out.println( q.dequeue( ) );
        System.out.println( q.dequeue( ) );
        System.out.println( q.dequeue( ) );
        System.out.println( q.dequeue( ) );
        System.out.println( q.dequeue( ) );
        // 1
        q.enqueue(new MyInteger(num++));
        q.enqueue(new MyInteger(num++));
        q.enqueue(new MyInteger(num++));
        // 4
        System.out.println( q.dequeue( ) );
        System.out.println( q.dequeue( ) );
        // 2
        q.enqueue(new MyInteger(num++));
        q.enqueue(new MyInteger(num++));
        // 4
        System.out.println( q.dequeue( ) );
      }
    catch( Overflow e ) { System.out.println( "Unexpected overflow" ); }
  }

}
