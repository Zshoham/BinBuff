package exceptions;

public class BufferOverflowException extends BufferException {

    public BufferOverflowException() {
    }

    public BufferOverflowException(String s) {
        super(s);
    }

    public BufferOverflowException(Throwable throwable) {
        super(throwable);
    }
}
