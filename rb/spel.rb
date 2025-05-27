require 'ruby2d'

puts "Välj nivå: lätt, medel eller svår"
level = gets.chomp.downcase

if level == 'lätt'
  time_limit = 30
elsif level == 'medel'
  time_limit = 20
elsif level == 'svår'
  time_limit = 10
else
  puts "Ogiltig nivå! Blir: medel"
  time_limit = 30
end

set title: "Asteroid Shooter", width: 2560, height: 1440

HITBOX_PADDING = 10
asteroids = []
score = 0
start_time = Time.now
last_spawn = Time.now
spawn_interval = 1.0

goal_text  = Text.new("Mål: Skjut ner 20 asteroider på #{time_limit} sekunder", x: 10, y: 10, size: 20, color: 'white')
score_text = Text.new("Poäng: #{score}", x: 10, y: 40, size: 20, color: 'white')
time_text  = Text.new("Tid: #{time_limit} sekunder", x: 10, y: 70, size: 20, color: 'white')

def spawn_asteroid(window_width, window_height)
  r = rand(20..40)
  side = rand(4)
  x = case side
      when 0 then rand(0..window_width) #uppifrån
      when 1 then rand(0..window_width) #nerifrån
      when 2 then -r #vänster
      else window_width + r #höger
      end
  y = case side
      when 0 then -r #uppifrån
      when 1 then window_height + r #nerifrån
      when 2 then rand(0..window_height) #vänster
      else rand(0..window_height) #höger
      end

  dx = (window_width/2)  - x
  dy = (window_height/2) - y
  dist = Math.sqrt(dx*dx + dy*dy)
  speed = rand(50..150) / 60.0
  vx = dx / dist * speed #omvandlar till vektor, dx "/" dist kommer bli något mellan -1 och 1, vi får ett förhållande för hur mycket den ska åka i x-led respektive y-led * en slumpmässig fart
  vy = dy / dist * speed

  shape = Circle.new(x: x, y: y,radius: r, sectors: 32, color: 'yellow') #kan ha hur många sectors dvs hörn som helst men räcker vid 32, blir en tydlig cirkel
  {shape: shape, vx: vx, vy: vy, radius: r} #skapa en hash med objektet för simplicitet, lättare än att göra en egen klass för objektet
end

on :mouse_down do |current|
  asteroids.reject! do |a| #gå igenom varje asteroid i listan asteroids, reject tar bort den från listan, inte bara visuellt
    dx = a[:shape].x - current.x
    dy = a[:shape].y - current.y
    if Math.sqrt(dx*dx + dy*dy) <= (a[:radius] + HITBOX_PADDING)
      a[:shape].remove
      score += 1
      score_text.text = "Poäng: #{score}"
      true
    else
      false
    end
  end
end

update do
  elapsed = (Time.now - start_time).to_i
  remaining = [time_limit - elapsed, 0].max #om remaining blir negativt sätter vi den till 0 istället
  time_text.text = "Tid: #{remaining} sekunder"

  if elapsed >= time_limit && score < 20
    puts "Spelet är slit - du misslyckades (som vanligt), du fick #{score} på #{time_limit} sekunder"
    Window.close
  end

  if Time.now - last_spawn >= spawn_interval
    asteroids << spawn_asteroid(Window.width, Window.height)
    last_spawn = Time.now
  end

  asteroids.reject! do |a|
    a[:shape].x += a[:vx]
    a[:shape].y += a[:vy]
    x = a[:shape].x
    y = a[:shape].y
    r = a[:radius]
    if x < -r || x > Window.width + r || y < -r || y > Window.height + r #om den har åkt för långt till vänster, höger, upp eller ner (0,0 längst upp till vänster)
      a[:shape].remove
      true
    else
      false
    end
  end

  if score >= 20
    puts "Grattis! Du vann på #{elapsed} sekunder"
    Window.close
  end
end

show