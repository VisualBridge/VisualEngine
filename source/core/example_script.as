// Made by claude, thanks llm.
class RotatingCube {
    Entity@ owner;
    float rotationSpeed = 50.0f;
    
    void SetOwner(Entity@ ent) {
        @owner = ent;
    }
    
    void OnInit() {
        print("RotatingCube initialized!");
        if (owner !is null) {
            print("Owner entity: " + owner.Name);
        }
    }
    
    void OnUpdate(float delta) {
        if (owner !is null) {
            TransformComponent@ transform = owner.getTransform();
            if (transform !is null) {
                transform.rotation.y += rotationSpeed * delta;
                
                // Wrap rotation
                if (transform.rotation.y > 360.0f) {
                    transform.rotation.y -= 360.0f;
                }
            }
        }
    }
    
    void SetRotationSpeed(float speed) {
        rotationSpeed = speed;
        print("Rotation speed set to: " + speed);
    }
}

class Oscillator {
    Entity@ owner;
    float time = 0.0f;
    float amplitude = 2.0f;
    float frequency = 1.0f;
    
    void SetOwner(Entity@ ent) {
        @owner = ent;
    }
    
    void OnInit() {
        print("Oscillator initialized!");
    }
    
    void OnUpdate(float delta) {
        time += delta;
        
        if (owner !is null) {
            TransformComponent@ transform = owner.getTransform();
            if (transform !is null) {
                transform.translation.y = sin(time * frequency) * amplitude;
            }
        }
    }
}