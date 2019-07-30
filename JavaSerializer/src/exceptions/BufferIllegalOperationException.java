package exceptions;

public class BufferIllegalOperationException extends BufferException {

    public BufferIllegalOperationException() {
        super("Attempted to preform an illegal operation in class Buffer.");
    }

    public BufferIllegalOperationException(String s) {
        super(s);
    }

    public BufferIllegalOperationException(Throwable throwable) {
        super(throwable);
    }

}
