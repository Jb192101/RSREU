import random

# Частотная вероятность
def simulate_ball_probability(num_red, num_blue, num_experiments):
    success_count = 0

    for _ in range(num_experiments):
        box = ['red'] * num_red + ['blue'] * num_blue

        random.shuffle(box)

        first_ball = box.pop()
        second_ball = box.pop()

        if first_ball == 'red' and second_ball == 'blue':
            success_count += 1

    frequency_probability = success_count / num_experiments
    return frequency_probability

red_balls = 10
blue_balls = 5
total_balls = red_balls + blue_balls

# Теоретическая вероятность
theoretical_prob = (red_balls / total_balls) * (blue_balls / (total_balls - 1))

num_simulations = 2000000
freq_prob = simulate_ball_probability(red_balls, blue_balls, num_simulations)

print(f"Классическая вероятность: {theoretical_prob:.6f} ({theoretical_prob:.2%})")
print(f"Частотная вероятность ({num_simulations} симуляций): {freq_prob:.6f} ({freq_prob:.2%})")
print(f"Разница между подходами: {abs(theoretical_prob - freq_prob):.6f}")