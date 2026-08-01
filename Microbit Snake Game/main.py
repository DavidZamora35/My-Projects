from microbit import *
import random

# Contains the coordinates for all parts of the snake's body. The first value is the head of the snake.
snake = [[0, 2], [1, 2]]
# Holds the (x, y) coordinate of the fruit.
fruit = [3, 2]
# Holds the direction of the snake.
direction = 0
# Flag for when a fruit is eaten.
eaten = False

# Moves the snake in the provided (integer) direction and returns said snake.
def move(snake, direction, eaten):
    temp = [snake[0][0], snake[0][1]]
    length = len(snake)
    # Right, down, left, up respectively
    if (direction%4 == 0):
        snake[0][0] += 1
    elif (direction%4 == 1):
        snake[0][1] += 1
    elif (direction%4 == 2):
        snake[0][0] -= 1
    elif (direction%4 == 3):
        snake[0][1] -= 1

    # If a fruit was eaten, insert new part where the head previously was.
    if (eaten):
        snake.insert(1, temp)
        eaten = False
    # Else, update the rest of the snake's body going backwards.
    elif (length <= 2):
        snake[1] = temp
    else:
        for idx in range(length-1, 1, -1):
            snake[idx] = snake[idx-1]
        snake[1] = temp

    return snake

# Generates a fruit on an empty tile.
def genFruit(snake):
    # List of values that the fruit cannot spawn on.
    exclusions = []
    
    for part in snake:
        # Convert coordinates of each body part to single interger value.
        exclusions += [(part[1]*5) + (part[0]+1)]

    # Generate fruit coordinates.
    fruitVal = random.choice(list(set([i for i in range(1, 26)]) - set(exclusions)))-1
    fruit = [fruitVal%5, fruitVal//5]

    return fruit

# Displays each segment of the snake on the board.
def displayBoard(snake, fruit):
    for part in snake:
        display.set_pixel(part[0], part[1], 9)
    display.set_pixel(fruit[0], fruit[1], 5)

# Checks if the snake's head is in an invalid position, returns True or False.
def invalid(snake):
    # If out of bounds, game over.
    if (snake[0][0] > 4 or snake[0][1] > 4 or snake[0][0] < 0 or snake[0][1] < 0):
        return True

    # If inside self, game over.
    for idx in range(1, len(snake)):
        if snake[idx] == snake[0]:
            return True

    return False
    

while True:
    display.clear()

    # A turns left, B turns right.
    if (button_a.was_pressed()):
        direction -= 1
    elif (button_b.was_pressed()):
        direction += 1

    # Move snake in given direction and reset "eaten" flag.
    # If a fruit was eaten, snake will increase in size.
    snake = move(snake, direction, eaten)
    eaten = False

    # If snake is full length of board, display win screen.
    if len(snake) == 25:
        display.scroll('YOU WIN!', delay=75)
        display.show(Image.FABULOUS)
        sleep(3000)
        reset()

    # If board state is invalid, game over.
    if (invalid(snake)):
        display.scroll('GAME OVER', delay=75)
        reset()

    # If on same tile as fruit, respawn fruit in random empty tile.
    if (snake[0][0] == fruit[0] and snake[0][1] == fruit[1]):
        fruit = genFruit(snake)
        eaten = True

    # Display the updated board state.
    displayBoard(snake, fruit)

    sleep(500)