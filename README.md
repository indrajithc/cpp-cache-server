
## Prerequisites

Before building the project, ensure you have the following dependencies installed:

- **C++ Compiler**: A modern C++ compiler (e.g., g++ or clang++)
- **CMake**: Build system generator
- **Boost Libraries**: For asynchronous networking
- **nlohmann/json**: For JSON parsing

### Installation Instructions

1. **Install Dependencies**:

   For Arch Linux, use:

   ```
   sudo apt install libboost-all-dev
   ```

   Install `nlohmann/json` by downloading it or using a package manager.

2. **Clone the Repository**:

   ```
   git clone https://github.com/your_username/cache-app-v1.git
   cd cache-app-v1/cpp_server
   ```

3. **Build the Project**:

   Create a build directory and compile the project:

   ```
   mkdir build
   cd build
   cmake ..
   make
   ```

## Usage

Run the server:

```
./cache_server
```

You can access the API at `http://localhost:5000/cache/`.

### API Endpoints

- **POST /cache**: Add a new cache entry.
- **GET /cache/{key}**: Retrieve a cache entry by key.
- **DELETE /cache/revalidate**: Invalidate cache entries based on tags.

## Example Request

To add a cache entry, you can use `curl`:

```
curl --location 'http://localhost:5000/cache' \
--header 'Content-Type: application/json' \
--data '{
  "key": "example_key",
  "value": "example_value",
  "tags": ["tag1", "tag2"]
}'
```

## Contributing

Contributions are welcome! Please create a pull request or open an issue.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
