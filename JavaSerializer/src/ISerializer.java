
public interface ISerializer<T> {

    void serialize(T data, Buffer buffer);
}
