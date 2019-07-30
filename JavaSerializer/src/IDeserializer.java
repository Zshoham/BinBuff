
public interface IDeserializer<T> {

    void deserialize(T dest, Buffer buffer);

    Class<T> getTypeClass();
}