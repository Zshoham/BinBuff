package exceptions;

public class BufferUnderflowException extends BufferException {

    public BufferUnderflowException() {
    }

    public BufferUnderflowException(String s) {
        super(s);
    }

    public BufferUnderflowException(Throwable throwable) {
        super(throwable);
    }
}
