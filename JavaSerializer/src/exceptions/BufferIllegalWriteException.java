package exceptions;

public class BufferIllegalWriteException extends BufferIllegalOperationException {

    public BufferIllegalWriteException() {
        super("cannot write to buffer while in read mode.");
    }

    public BufferIllegalWriteException(String s) {
        super(s);
    }

    public BufferIllegalWriteException(Throwable throwable) {
        super(throwable);
    }
}
