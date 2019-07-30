package exceptions;

public class BufferException extends RuntimeException {

    public BufferException() {
        super("Exception in class Buffer, cause unknown.");
    }

    public BufferException(String s) {
        super(s);
    }

    public BufferException(Throwable throwable) {
        super(throwable);
    }

    @Override
    public String getMessage() {
        return super.getMessage();
    }

    @Override
    public synchronized Throwable getCause() {
        return super.getCause();
    }
}
