package exceptions;

public class BufferIllegalReadException extends BufferIllegalOperationException {

    public BufferIllegalReadException() {
        super("cannot read from buffer while in read mode.");
    }

    public BufferIllegalReadException(String s) {
        super(s);
    }

    public BufferIllegalReadException(Throwable throwable) {
        super(throwable);
    }

}
