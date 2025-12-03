#version 330 core

// 给光源数据一个结构体
struct Light {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec3 position;
};

// 给物体材质数据一个结构体
struct Material {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};

// 相机坐标
uniform vec3 eye_position;
// 光源
uniform Light light;
// 物体材质
uniform Material material;
uniform int isShadow;
uniform int isGreen;

in vec3 position;
in vec3 normal;
in vec3 color;
in vec2 textureCoords;

out vec4 fColor;

uniform sampler2D textureVar;

uniform int useTexture;

void main() {
	if(isShadow == 1) {
		fColor = vec4(0.0, 0.0, 0.0, 1.0);
	} else {
		// 法向量归一化
		vec3 norm = normalize(normal);

		// @TODO: 计算四个归一化的向量 N,V,L,R(或半角向量H)
		vec3 V = normalize(eye_position - position);
		vec3 L = normalize(light.position - position);
		vec3 R = reflect(-L, norm);
		vec3 N = norm;

		// 环境光分量I_a
		vec4 I_a = light.ambient * material.ambient;

		// @TODO: Task2 计算系数和漫反射分量I_d
		float diffuse_dot = max(dot(L, N), 0.0);
		vec4 I_d = diffuse_dot * light.diffuse * material.diffuse;

		// @TODO: Task2 计算系数和镜面反射分量I_s
		float specular_dot_pow = pow(max(dot(R, V), 0.0), material.shininess);
		vec4 I_s = specular_dot_pow * light.specular * material.specular;

		if(dot(L, N) < 0.0) {
			I_s = vec4(0.0, 0.0, 0.0, 1.0);
		}

		// 合并三个分量的颜色，修正透明度
		if(isGreen == 1) {
			fColor = vec4(0.06, 0.49, 0.06, 1.) * (I_a + I_d + I_s);//texture(textureVar, textureCoords);
		} else if(useTexture == 1) {
			fColor = (I_a + I_d + I_s) * texture(textureVar, textureCoords);
			//fColor = texture(textureVar, textureCoords);
		} else {
			fColor = (I_a + I_d + I_s) * vec4(color, 1.0);
		}
	}
}
