import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.lang.reflect.*;
import java.net.URL;
import java.net.URLClassLoader;
import java.util.*;
import java.util.jar.JarEntry;
import java.util.jar.JarFile;

public class JarReverseEngineeringTool {
    
    private static final Set<String> processedClasses = new HashSet<>();
    private static final Map<String, Set<String>> classRelationships = new HashMap<>();
    private static PrintWriter outputWriter;
    
    public static void main(String[] args) {
        if (args.length < 1) {
            System.out.println("Usage: java JarReverseEngineeringTool <path-to-jar-file>");
            return;
        }
        
        String jarPath = args[0];
        File jarFile = new File(jarPath);
        
        if (!jarFile.exists() || !jarFile.getName().endsWith(".jar")) {
            System.out.println("Error: File does not exist or is not a JAR file.");
            return;
        }
        
        // Create output file name based on the JAR file name
        String outputFileName = jarFile.getName().replace(".jar", "") + "_analysis.txt";
        
        try {
            // Initialize the output writer
            outputWriter = new PrintWriter(new FileWriter(outputFileName));
            
            // Print header information
            outputWriter.println("===============================================");
            outputWriter.println("JAR Reverse Engineering Analysis");
            outputWriter.println("JAR File: " + jarFile.getAbsolutePath());
            outputWriter.println("Date: " + new Date());
            outputWriter.println("===============================================\n");
            
            analyzeJarFile(jarFile);
            
            System.out.println("Analysis complete. Results saved to: " + outputFileName);
        } catch (Exception e) {
            System.out.println("Error analyzing JAR file: " + e.getMessage());
            e.printStackTrace();
        } finally {
            if (outputWriter != null) {
                outputWriter.close();
            }
        }
    }
    
    private static void analyzeJarFile(File jarFile) throws IOException {
        JarFile jar = new JarFile(jarFile);
        URL[] urls = {new URL("file:" + jarFile.getAbsolutePath())};
        
        try (URLClassLoader classLoader = new URLClassLoader(urls)) {
            Enumeration<JarEntry> entries = jar.entries();
            
            List<String> classNames = new ArrayList<>();
            Map<String, Class<?>> loadedClasses = new HashMap<>();
            
            // First pass: collect class names
            while (entries.hasMoreElements()) {
                JarEntry entry = entries.nextElement();
                if (entry.getName().endsWith(".class")) {
                    String className = entry.getName().replace('/', '.').replace(".class", "");
                    classNames.add(className);
                }
            }
            
            outputWriter.println("Found " + classNames.size() + " classes in the JAR file.\n");
            
            // Second pass: load classes safely
            for (String className : classNames) {
                try {
                    Class<?> clazz = classLoader.loadClass(className);
                    loadedClasses.put(className, clazz);
                } catch (Throwable e) {
                    outputWriter.println("Warning: Could not load class " + className);
                }
            }
            
            outputWriter.println("Successfully loaded " + loadedClasses.size() + " classes.\n");
            outputWriter.println("===============================================");
            outputWriter.println("CLASS DEFINITIONS");
            outputWriter.println("===============================================\n");
            
            // Third pass: analyze classes and their relationships
            for (Map.Entry<String, Class<?>> entry : loadedClasses.entrySet()) {
                try {
                    analyzeClassSafely(entry.getValue());
                } catch (Throwable e) {
                    outputWriter.println("Error analyzing class " + entry.getKey() + ": " + e.getMessage());
                }
            }
            
            // Display relationships
            outputWriter.println("\n===============================================");
            outputWriter.println("CLASS RELATIONSHIPS");
            outputWriter.println("===============================================\n");
            
            for (Map.Entry<String, Set<String>> entry : classRelationships.entrySet()) {
                String className = entry.getKey();
                Set<String> relationships = entry.getValue();
                
                if (!relationships.isEmpty()) {
                    outputWriter.println("\n" + className + " has relationships with:");
                    for (String relationship : relationships) {
                        outputWriter.println("  " + relationship);
                    }
                }
            }
        } catch (Exception e) {
            outputWriter.println("Error creating URLClassLoader: " + e.getMessage());
            e.printStackTrace(outputWriter);
        }
        
        jar.close();
    }
    
    private static void analyzeClassSafely(Class<?> clazz) {
        String className = clazz.getName();
        
        // Skip if already processed or if it's a Java standard library class
        if (processedClasses.contains(className) || className.startsWith("java.") || className.startsWith("javax.")) {
            return;
        }
        
        processedClasses.add(className);
        classRelationships.put(className, new HashSet<>());
        
        outputWriter.println("\n===================================");
        
        // Class or Interface declaration
        outputWriter.print(Modifier.toString(clazz.getModifiers()) + " ");
        if (clazz.isInterface()) {
            outputWriter.print("interface ");
        } else {
            outputWriter.print("class ");
        }
        outputWriter.println(className);
        
        try {
            // Superclass
            Class<?> superClass = clazz.getSuperclass();
            if (superClass != null && !superClass.equals(Object.class)) {
                outputWriter.println("  extends " + superClass.getName());
                addRelationship(className, "extends " + superClass.getName());
            }
        } catch (Throwable e) {
            outputWriter.println("  // Error retrieving superclass: " + e.getMessage());
        }
        
        try {
            // Interfaces
            Class<?>[] interfaces = clazz.getInterfaces();
            if (interfaces.length > 0) {
                outputWriter.print("  implements ");
                for (int i = 0; i < interfaces.length; i++) {
                    outputWriter.print(interfaces[i].getName());
                    addRelationship(className, "implements " + interfaces[i].getName());
                    if (i < interfaces.length - 1) {
                        outputWriter.print(", ");
                    }
                }
                outputWriter.println();
            }
        } catch (Throwable e) {
            outputWriter.println("  // Error retrieving interfaces: " + e.getMessage());
        }
        
        outputWriter.println("{");
        
        try {
            // Fields
            outputWriter.println("  // Fields");
            Field[] fields = clazz.getDeclaredFields();
            for (Field field : fields) {
                try {
                    outputWriter.println("  " + Modifier.toString(field.getModifiers()) + " " + 
                                    field.getType().getName() + " " + field.getName() + ";");
                    
                    // Record type relationships for complex types (not primitives)
                    Class<?> fieldType = field.getType();
                    if (!fieldType.isPrimitive() && !fieldType.getName().startsWith("java.lang")) {
                        addRelationship(className, "has field of type " + fieldType.getName());
                    }
                } catch (Throwable e) {
                    outputWriter.println("  // Error analyzing field: " + field.getName());
                }
            }
        } catch (Throwable e) {
            outputWriter.println("  // Error retrieving fields: " + e.getMessage());
        }
        
        try {
            // Constructors
            outputWriter.println("\n  // Constructors");
            Constructor<?>[] constructors = clazz.getDeclaredConstructors();
            for (Constructor<?> constructor : constructors) {
                try {
                    outputWriter.print("  " + Modifier.toString(constructor.getModifiers()) + " " + 
                                    className.substring(className.lastIndexOf('.') + 1) + "(");
                    
                    Class<?>[] paramTypes = constructor.getParameterTypes();
                    for (int i = 0; i < paramTypes.length; i++) {
                        outputWriter.print(paramTypes[i].getName() + " param" + i);
                        if (i < paramTypes.length - 1) {
                            outputWriter.print(", ");
                        }
                        
                        // Record parameter relationships
                        if (!paramTypes[i].isPrimitive() && !paramTypes[i].getName().startsWith("java.lang")) {
                            addRelationship(className, "has constructor parameter of type " + paramTypes[i].getName());
                        }
                    }
                    outputWriter.println(");");
                } catch (Throwable e) {
                    outputWriter.println("  // Error analyzing constructor");
                }
            }
        } catch (Throwable e) {
            outputWriter.println("  // Error retrieving constructors: " + e.getMessage());
        }
        
        try {
            // Methods
            outputWriter.println("\n  // Methods");
            Method[] methods;
            try {
                methods = clazz.getDeclaredMethods();
            } catch (NoClassDefFoundError e) {
                outputWriter.println("  // Could not access methods due to missing dependencies: " + e.getMessage());
                methods = new Method[0];
            }
            
            for (Method method : methods) {
                try {
                    outputWriter.print("  " + Modifier.toString(method.getModifiers()) + " " + 
                                   method.getReturnType().getName() + " " + method.getName() + "(");
                    
                    Class<?>[] paramTypes = method.getParameterTypes();
                    for (int i = 0; i < paramTypes.length; i++) {
                        outputWriter.print(paramTypes[i].getName() + " param" + i);
                        if (i < paramTypes.length - 1) {
                            outputWriter.print(", ");
                        }
                        
                        // Record parameter relationships
                        if (!paramTypes[i].isPrimitive() && !paramTypes[i].getName().startsWith("java.lang")) {
                            addRelationship(className, "has method parameter of type " + paramTypes[i].getName());
                        }
                    }
                    outputWriter.println(");");
                    
                    // Record return type relationship
                    Class<?> returnType = method.getReturnType();
                    if (!returnType.isPrimitive() && !returnType.equals(void.class) && 
                        !returnType.getName().startsWith("java.lang")) {
                        addRelationship(className, "returns " + returnType.getName());
                    }
                } catch (Throwable e) {
                    outputWriter.println("  // Error analyzing method: " + method.getName());
                }
            }
        } catch (Throwable e) {
            outputWriter.println("  // Error retrieving methods: " + e.getMessage());
        }
        
        outputWriter.println("}");
    }
    
    private static void addRelationship(String className, String relationship) {
        Set<String> relationships = classRelationships.get(className);
        if (relationships != null) {
            relationships.add(relationship);
        }
    }
}